#include "analyzer.h"

#include <float.h>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
//#include <QString>
/*** Directives for the Grace library ***/
//#include <unistd.h>
// #include <grace_np.h>

#include <math.h>

/*** Levenberg-Marquardt algorithm, LEVMAR ***/
#include "lm.h"

#define MAGIC_TANH 2.0	//value to divide tanh by, in order to let it reach values of 0.0 and 1.0 exactly (should be around 2.0).

// void my_error_function(const char *msg) { fprintf(stderr, "[GRACE library]: \"%s\"\n", msg); }

double p [NUM_PARAMS]; 	//tablica aktualnych wartosci wszystkich parametrow (czesc nie bedzie sie zmieniac)
int permut [NUM_PARAMS]; 	//tablica permutacyjna: permut[i] - ktory element z wektora p znajduje sie na i-tej pozycji w param.Grace
int reverse_permut [NUM_PARAMS];//odwrotne tablica permutacyjna: permut[a] - ktory numer w tablicy permut ma a-ty parametr z programu glownego

double baseline = 1;

double *x_data; //x_data - zmienna niezalezna (czas)
double *y_data; //y_data - G(t) EKSPERYMENTALNE!

double Par (int i) {	//calculate the actual value of a parameter
  switch (i) {
	 case 2: case 5: case 7: case 10:	//T, A1, A2, B	e (0,1)
		return tanh(p[i])/MAGIC_TANH + 0.5;break;
/*	 case 3:				//t_T max = 10 000 us
		ub[i] = 1e4; break;
	 case 9:
		ub[i] = 1e7; break; 		//t_bl max = 10s*/
	 default:
		return p[i]*p[i];break;		//zwracamy kwadrat --> Par(i) >= 0 :-)
  }
}

void G_function (double *lmp, double *hy, int m, int n, void *adata) {	//hy - G(t) DOPASOWANE! lmp- wart parametrow wg L-M

	//przepisz nowe wartosci parametrow do wektora p wg tablicy permutacyjnej
	for (int i=0; i<m; i++) p[permut[i]] = lmp[i];
	for (int i=0; i<n; i++) hy[i] = G_val (x_data[i],NULL) - y_data[i];
}


//Oblicza wartosc funkcji dla zadanego opoznienia t i aktualnych parametrow
double G_val (double t, const double new_params [NUM_PARAMS] = NULL) {

 if (new_params == NULL)   return  
   Par(0) * (1 + Par(2)*exp(-t/Par(3)) - Par(2)) / (1 - Par(2))		  	//triplet
	* (1 + Par(10)*exp(-t/Par(9)) - Par(10)) 	     		  	//bleaching
	* ( Par(5) / (1+t/Par(4)) / sqrt(1 + t/(Par(1)*Par(1)*Par(4)))  		//1st diffusion
	  + Par(7) / (1+t/Par(6)) / sqrt(1 + t/(Par(1)*Par(1)*Par(6)))  		//2nd diffusion
	  + (1-Par(5)-Par(7)) / (1+t/Par(8)) / sqrt(1 + t/(Par(1)*Par(1)*Par(8)))  	//3rd diffusion
	) + baseline;						// +1, bo nieznormalizowana g(2)

 else return  
   new_params[0] * (1 + new_params[2]*exp(-t/new_params[3]) - new_params[2]) / (1 - new_params[2])		  	//triplet
	* (1 + new_params[10]*exp(-t/new_params[9]) - new_params[10]) 	     		  				//bleaching
	* (new_params[5] / (1+t/new_params[4]) / sqrt(1 + t/(new_params[1]*new_params[1]*new_params[4]))  		//1st diffusion
	  + new_params[7] / (1+t/new_params[6]) / sqrt(1 + t/(new_params[1]*new_params[1]*new_params[6]))  		//2nd diffusion
	  + (1-new_params[5]-new_params[7]) / (1+t/new_params[8]) / sqrt(1 + t/(new_params[1]*new_params[1]*new_params[8]))  	//3rd diffusion
	) + baseline;						// +1, bo nieznormalizowana g(2)

}

// static double Pochodna (int a, double t) {
// //TODO przepisac rownania z maximy :)
// 
// 	 //zm. pomocnicze dla przyspieszenia obliczen
// 	 double Gv = G_val (t);
// 	 double nawias_T = (1 + p[2]*exp(-t/p[3]) - p[2]);
// 	 double nawias_B = (1 + p[9]*exp(-t/p[9]) - p[9]);
// 	 double nawias_1 = p[5] / (1+t/p[4]) / sqrt(1 + t/(p[1]*p[1]*p[4]));
// 	 double nawias_2 = p[7] / (1+t/p[6]) / sqrt(1 + t/(p[1]*p[1]*p[6]));
// 	 double nawias_3 = (1-p[5]-p[7]) / (1+t/p[8]) / sqrt(1 + t/(p[1]*p[1]*p[8]));
// 	 double nawiasy_123 = nawias_1 + nawias_2 + nawias_3;
// 
// 	if (a==0) return Gv/p[0];				//dN
// 	else if (a==1) return nawias_T * nawias_B * p[0] * (	//dSP
// 	 ((-p[7]-p[5]+1)*t) / (p[1]*p[1]*p[1]*p[8]*(t/p[8]+1)*(t/(p[1]*p[1]*p[8])+1)*sqrt(t/(p[1]*p[1]*p[8])+1)) +
// 	  (p[7]*t) / (p[1]*p[1]*p[1]*p[6]*(t/p[6]+1)*(t/(p[1]*p[1]*p[6])+1)*sqrt(t/(p[1]*p[1]*p[6])+1)) +
// 	  (p[5]*t) / (p[1]*p[1]*p[1]*p[4]*(t/p[4]+1)*(t/(p[1]*p[1]*p[4])+1)*sqrt(t/(p[1]*p[1]*p[4])+1)) );
// 	else if (a==2) return p[0]*nawiasy_123*nawias_B *	//dT
// 	 (exp(-t/p[3]) - 1);
// 	else if (a==3) return p[0]*p[2]*t*nawiasy_123*nawias_B*exp(-t/p[3])/p[3]/p[3]; //dt_T
// 	else if (a==4) return p[0]*nawias_T*nawias_B *		//dt_D1  //CHECK! TODO nie wiem czy dziala jak nalezy :/
// 			(nawias_1*t/p[4]/p[4]/(1+t/p[4]) +
// 			 nawias_1*t/p[4]/p[4]/2/(1+t/p[4]/p[1]/p[1])/p[1]/p[1]) ;
// 	else if (a==5) return p[0]*nawias_T*nawias_B *		//dA1
// 			(nawias_1/p[5]-nawias_3/(1-p[5]-p[7])) ;
// 	else if (a==6) return p[0]*nawias_T*nawias_B *		//dt_D2
// 			(nawias_2*t/p[6]/p[6]/(1+t/p[6]) +
// 			 nawias_2*t/p[6]/p[6]/2/(1+t/p[6]/p[1]/p[1])/p[1]/p[1]) ;
// 	else if (a==7) return p[0]*nawias_T*nawias_B * 		//dA2
// 			(nawias_2/p[7]*-nawias_3/(1-p[5]-p[7])) ;
// 	else if (a==8) return p[0]*nawias_T*nawias_B *		//dt_D3
// 			(nawias_3*t/p[8]/p[8]/(1+t/p[8]) +
// 			 nawias_3*t/p[8]/p[8]/2/(1+t/p[8]/p[1]/p[1])/p[1]/p[1]) ;
// 	else if (a==9) return  p[0]*p[10]*t*nawiasy_123*nawias_T*exp(-t/p[9])/p[9]/p[9]; //dt_bl
// 	else if (a==10) return p[0]*nawias_T*nawiasy_123*(exp(-t/p[9])-1) ; //dB
// 	else {std::cerr << "Invalid parameter number!"; exit (2);}
// }
// 
// // Wypelnij tablice z Jakobianem na poczet algorytmu L-M
// void Jacobian (double *lmp, double *jac, int m, int n, void *adata) {
//   for (int i=0; i<m; i++) p[permut[i]] = lmp[i]; //update real parameters list, just in case
// 
//   int i,j,k;
// 
//   for(i=j=0; i<n; ++i){		//i - kolejne punkty, j - indeks w tabeli jakobianu (1-wymiarowa :/ )
//     for (k=0; k<m; k++) {	//k - kolejne parametry
// 	jac[j++]=Pochodna(permut[k], x_data[i]);
//     }
//   }
// }
// 

/***********************************************
 * Przerobiony na wykorzystanie pakietu levmar *
 ***********************************************/
bool Analyzer :: Fit () {

 printf ("atanh(0.999->) = %lf, %lf, %lf",atanh(0.999),atanh(0.9999), atanh(0.999999));
printf ("atanh(0.001->) = %lf, %lf, %lf",atanh((0.001-0.5)*MAGIC_TANH),atanh((0.00001-0.5)*MAGIC_TANH), atanh((0.0000001-0.5)*MAGIC_TANH));

  baseline = inputBaseLine->text().toDouble();

  if (!CalculateAverageCorrelation())  //leave only one (average) function & check if there's anything to fit to :)
	{ printf ("Average correlation could not be calculated.\n"); return false;}

	/********************************************************************************
	 * p[i] - i-ty parametr (w tabeli zwyklej - z formularza)			*
	 * permut[i] - co oznacza i-ty parametr sposrod modyfikowalnych			*
	 * reverse_permut[i] - ktory nr w tabeli permutacyjnej ma normalny parametr i 	*
	 ********************************************************************************/
  int num_par = 0;		//number of fitted parameters

  for (int i = 0; i < NUM_PARAMS; i++) {
	p[i] = tableParameters->item(i,0)->text().toDouble();
	if (tableParameters->item(i,0)->background() != Qt::gray) {	//jezeli mamy dopasowywac dany parametr
		permut[num_par] = i;					//to wpisujemy go do tablicy permutacyjnej
		reverse_permut[i] = num_par;
		num_par++;
	}
	else reverse_permut[i] = -1;
  }


for (int i=0; i<num_par; i++) printf ("permut [%d] = %d --> %lf\n",i,permut[i],p[permut[i]]);

  //box constraints - na nowa modle (funkcje tanh, x^2)

  for (int i=0; i<NUM_PARAMS; i++) {	//odwrocon parametry (pierwiastki, atanh itp.) do tablicy p
	switch (i) {
	    case 2: case 5: case 7: case 10:	//T, A1, A2, B	e (0,1)
		printf ("Converting p[%i]... %lf -->",i,p[i]);
		p[i] = atanh ((p[i]-0.5)*MAGIC_TANH);
		    break;
	    default:
		p[i] = sqrt(p[i]);break;	//reszta >= 0
	}
   }


for (int i=0; i<NUM_PARAMS; i++) printf ("Converted param %i: %lf\n",i,p[i]);

  if (num_par == 0) {fprintf (stderr, "Error: no free parameters for fit.\n"); return false;}

  //Load curve data into the arrays
  QList<QwtPlotItem*> lista = qwtPlotCF->itemList();
  if (lista.size()!=1) {printf ("Warning: more than one curve on the plot!!\n");}

  QwtPlotCurve* krzywa = (QwtPlotCurve*) lista[0]; //dopasowujemy do I krzywej z wykresu
  const uint start = fitFrom->text().toUInt();
  const uint n = fitTo->text().toUInt() - fitFrom->text().toUInt();//krzywa->dataSize(); 	//n-dlugosc traj TODO

  printf ("\nnum_par = %d, n = %d (od %d do %d)\n",num_par,n,start,fitTo->text().toUInt()); //debug
  if (n < num_par) {printf ("Too few data points (n<p), cannot fit!\n"); return false;}

  //przepiszmy punkty ekperymentalne z wykresu do tablic globalnych x[] i y[]
   x_data = new double [n];
   y_data = new double [n];

  for (uint i = 0; i < n; i++) { x_data[i] = krzywa->sample(start+i).x(); y_data[i] = krzywa->sample(start+i).y();}

  //LEVMAR: m - liczba parametrow, n - liczba punktow
//      double *work, *covar;
//      work=(double*)malloc((LM_DIF_WORKSZ(num_par, n)+num_par*num_par)*sizeof(double));
//      if(!work){
//          fprintf(stderr, "memory allocation request failed in main()\n");
//        exit(1);
//      }
//      covar=work+LM_DIF_WORKSZ(num_par, n);

//      ret=dlevmar_dif(G_function, p, y, m, n, 1000, opts, info, work, covar, NULL); // no Jacobian, caller allocates work memory, covariance estimated

   double p_robocze [num_par]; for (int i=0; i<num_par; i++) p_robocze[i] = p[permut[i]]; //na tych parametrach algorytm LM bedzie operowal

   int ret;
   double opts[LM_OPTS_SZ], info[LM_INFO_SZ]; //opts musi byc wypelnione!!

   //domyslnie:
   //opts[0] = 1e-3; opts[1] = opts[2] = 1e-17; opts[3] = 1e-17; opts[4] = 1e-6 //4 -tylko dla "_dif"
   opts[0] = 1e-2; opts[1] = opts[2] = 1e-50; opts[3] = 1e-17; opts[4] = 1e-6; //4 -tylko dla "_dif"


   // tu musimy przekazac p_robocze, a nie p !! zeby nie zmienial nam bezposrednio w nszej tabelce, a jedynie po uwzglednieniu permutacji!
    ret = dlevmar_dif(G_function, p_robocze, NULL, num_par, n, 100, NULL, info, NULL, NULL, NULL);
//     ret = dlevmar_bc_der(G_function, Jacobian, p_robocze, y_data, num_par, n, lb, ub, 100, NULL, info, NULL, NULL, NULL);
//    ret = dlevmar_der(G_function, Jacobian, p_robocze, y_data, num_par, n, 1000, NULL, info, NULL, NULL, NULL); // no Jacobian, caller allocates work memory, covariance estimated

   printf("Levenberg-Marquardt returned in %g iter, reason %g, sumsq %g [%g]\n", info[5], info[6], info[1], info[0]);
   printf("Best fit parameters:\n"); for (int i=0;i<num_par;i++) printf ("%d: %7.7g\n",i,p[permut[i]]);
   printf("Best fit parameters:\n"); for (int i=0;i<NUM_PARAMS;i++) printf ("p[%d]: %7.7g\n",i,p[i]);	//wypiszmy wszystkie parametry (takze te zafixowane)

//   double err[n];
//   dlevmar_chkjac(G_function, Jacobian, p_robocze, num_par, n, NULL,err);
//   for (int i=0;i<n;i++) printf ("err[%d] = %lf\n",i,err[i]); //output the errors between numer and analytical Jacobians

//     printf("Covariance of the fit:\n");
//     for(i=0; i<num_par; ++i){
//       for(j=0; j<num_par; ++j)
//         printf("%g ", covar[i*m+j]);
//       printf("\n");
//     }
//     printf("\n");

//      free(work);

  delete [] x_data; delete [] y_data;
  //display results back in tableParameters
  for (uint i = 0; i < NUM_PARAMS; i++) tableParameters->item(i,0)->setText(QString::number(Par(i)));

  for (int i=0; i<num_par; i++) printf ("permut [%d] = %d --> %lf\n",i,permut[i],p[permut[i]]);

  DrawCorrelationFromParams();

  /*** Ocena jakości fitu: [chi2 = suma{(yi-yi_fit)^2} x (N^2) / n] ***/
  double G0 = tableParameters->item(0,0)->text().toDouble();
  double chi2n = info[1]/G0/G0/n;
  lblChiSqr->setText (QString::number(chi2n,'e'));
  
  return true;
}


/*
static double pochodna_numer (int a, double t) { //FIXME nie dziala? TODO
	const double wartosc = p[a];
	const double h = 0.1*wartosc; //krok
	p[a] = wartosc + h;
	const double G_1 = G_val (t);
	p[a] = wartosc - h;
	const double G_2 = G_val (t);
	p[a] = wartosc;	//przywracamy stara wartosc a-tego parametru
	return (G_1-G_2)/2/h;	//3-pktowa
}


//Wartosc pochodnej po parametrze a w punkcie t

static int G_f (const gsl_vector * par_fit, void *data, gsl_vector * f) {
std::cout << "G_F! ";
	uint n = ((struct data*) data)->n;
	double *y = ((struct data *)data)->y;
	double *x = ((struct data*) data)->x;
	double *sigma = ((struct data *) data)->sigma;

	//uaktualniamy nowe dopasowane wartosci
        for (uint i = 0; i < num_par; i++) p[permut[i]] = gsl_vector_get (par_fit,i);

	for (uint i = 0; i < n; i++)
         {
           double t = x[i];
           double Yi = G_val (t);
           gsl_vector_set (f, i, (Yi - y[i])/sigma[i]);
         }

	return GSL_SUCCESS;
}



//*/
// QString Analyzer :: Par (int i) {
//   if (tableParameters->item(i,0)->background() == Qt::gray) return tableParameters->item(i,0)->text(); //jezeli param jest zablokowany, to wpisujemy do rownania jego wartosc
// 
//   //jezeli dopasowujemy parametr - trezba go wpisac jako A0 / A1 /... itp.
//   QString str = "A";
//   str += QString::number (reverse_permut[i]);
//   return str;
// }


///////////////
/*
    GraceRegisterErrorFunction(my_error_function);

    // Start Grace with a buffer size of 2048 and open the pipe 
    if (GraceOpen(2048) == -1) { fprintf(stderr, "Can't run Grace. \n"); exit(EXIT_FAILURE); }
    
    GracePrintf("s0 on");
    GracePrintf("title \"FCS fit\"");
    GracePrintf("s0 symbol 1");
    GracePrintf("s0 symbol size 0.2");
    GracePrintf("s0 symbol fill pattern 1");

    // Send plot data to Grace
    for (uint i = 0; i < n && GraceIsOpen(); i++) GracePrintf("g0.s0 point %lf, %lf", krzywa->x(start+i), krzywa->y(start+i));
     //	std::cout << x[i] << ", " << y[i] <<"\n";

    if (GraceIsOpen()) {
        GracePrintf("xaxes scale logarithmic \n autoscale \n saveall \"sample.agr\"");
	
	QString s = "fit formula \"y = ";

	//here comes our formula: triplet, bleaching, 1st, 2nd, 3rd diffusion, +1 to normalize g(2)
        s += ""+Par(0)+" * (1+"+Par(2)+"*exp(-x/"+Par(3)+")-"+Par(2)+")/(1-"+Par(2)+")\
 * (1+"+Par(10)+"*exp(-x/"+Par(9)+")-"+Par(10)+")\
 * ("+Par(5)+"/(1+x/"+Par(4)+")/sqrt(1+x/("+Par(1)+"*"+Par(1)+"*"+Par(4)+"))\
 + "+Par(7)+"/(1+x/"+Par(6)+")/sqrt(1+x/("+Par(1)+"*"+Par(1)+"*"+Par(6)+"))\
 + (1-"+Par(5)+"-"+Par(7)+")/(1+x/"+Par(8)+")/sqrt(1+x/("+Par(1)+"*"+Par(1)+"*"+Par(8)+"))\
 ) + 1";

	s += "\" \n";

	GracePrintf(s.toAscii());

	printf (s.toAscii());
	
	s = "fit with ";
	s += QString::number(num_par);
	s += " parameters\n fit prec 0.05 \n";		// \n needed?

//	   std::cout << s.toStdString();
	GracePrintf(s.toAscii());

	//write out initial parameters to Grace
        for (uint i = 0; i < num_par; i++) {
	   s = "";

	   //Special cases: constraints! Ograniczenia dla czasow: 0-100000000us, ogr. dla ulamkow: 0-1
	   if (permut[i]==0 || permut[i]==1 || permut[i]==3 || permut[i]==4 || permut[i]==6 || permut[i]==8 || permut[i]==9) 
		 s += "A" + QString::number(i) + " = " + QString::number(p[permut[i]]) + "\n A" + QString::number(i)+" constraints on\n A"+QString::number(i)+"min = 0 \n A"+QString::number(i)+"max = 1e8\n";
	   else 
		 s += "A" + QString::number(i) + " = " + QString::number(p[permut[i]]) + "\n A" + QString::number(i)+" constraints on\n A"+QString::number(i)+"min = 0 \n A"+QString::number(i)+"max = 1\n";

// 	  printf (s.toAscii());

	   GracePrintf(s.toAscii());	   
	}

	//Perform fit
	s = ""; s += "nonlfit (s0, "+QString::number(n)+")\n";
        GracePrintf (s.toAscii());

	//Store fit parameters in a new dataset
	s = "s1 on \n s1 length ";
	s += QString::number (num_par);
        for (uint i = 0; i < num_par; i++) 
	  s += "\ns1.x["+QString::number(i)+"] = " + QString::number(i) + "\ns1.y["+QString::number(i)+"] = A"+QString::number(i);
        GracePrintf (s.toAscii());

	//save the dataset with parameters to a file
	GracePrintf ("WRITE s1 FILE \"par.fit\"");
	GraceClose();

	//add info which param is which
	FILE * fpar = fopen ("par.fit","a");
	for (uint i=0;i<num_par;i++) {
		fprintf (fpar, "%d %s\n", i, tableParameters->verticalHeaderItem(permut[i])->text().data());
	}
	fclose(fpar);
    } 
    else exit(EXIT_FAILURE);
    
  //back to our program: read params file & update our variables
  std::ifstream fpar ("par.fit");
  for (uint i = 0; i < num_par; i++) { fpar >> p[permut[i]]; fpar >> p[permut[i]];} //HACK pierwsza liczba jest niewazna (numer parametru), wiec ja nadpiszemy
*/
