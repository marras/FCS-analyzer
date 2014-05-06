#include "analyzer.h"

#include <stdlib.h>
#include <stdio.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_multifit_nlin.h>

//#define N 40
#define MAX_ITERS 50

uint num_par = 0;		 //number of fitted parameters
double p [NUM_PARAMS]; 		//tablica aktualnych wartosci wszystkich parametrow (czesc nie bedzie sie zmieniac)
double par_init [NUM_PARAMS];   //poczatkowe guessy - tylko fitowane (po permutacji)
int permut [NUM_PARAMS]; 	//tablica permutacyjna: permut[i] - ktory element z wektora p znajduje sie na i-tej pozycji w par_init

struct data {
       uint n;
       double * x;
       double * y;
       double * sigma;
};

static void print_state (size_t iter, gsl_multifit_fdfsolver * s)   {
  printf ("iter: %3u x = % 15.8f "
          "|f(x)| = %g\n",
          iter, gsl_vector_get (s->x, 0), gsl_blas_dnrm2 (s->f)); /////DEBUG, TODO poprawic
}

//Oblicza wartosc funkcji dla zadanego opoznienia t i aktualnych parametrow
double G_val (double t, const double new_params [NUM_PARAMS] = NULL) {

 if (new_params == NULL)   return  
   p[0] * (1 + p[2]*exp(-t/p[3]) - p[2]) / (1 - p[2])		  	//triplet
	* (1 + p[10]*exp(-t/p[9]) - p[10]) 	     		  	//bleaching
	* ( p[5] / (1+t/p[4]) / sqrt(1 + t/(p[1]*p[1]*p[4]))  		//1st diffusion
	  + p[7] / (1+t/p[6]) / sqrt(1 + t/(p[1]*p[1]*p[6]))  		//2nd diffusion
	  + (1-p[5]-p[7]) / (1+t/p[8]) / sqrt(1 + t/(p[1]*p[1]*p[8]))  	//3rd diffusion
	) + 1;						// +1, bo nieznormalizowana g(2)

 else return  
   new_params[0] * (1 + new_params[2]*exp(-t/new_params[3]) - new_params[2]) / (1 - new_params[2])		  	//triplet
	* (1 + new_params[10]*exp(-t/new_params[9]) - new_params[10]) 	     		  	//bleaching
	* (new_params[5] / (1+t/new_params[4]) / sqrt(1 + t/(new_params[1]*new_params[1]*new_params[4]))  		//1st diffusion
	  + new_params[7] / (1+t/new_params[6]) / sqrt(1 + t/(new_params[1]*new_params[1]*new_params[6]))  		//2nd diffusion
	  + (1-new_params[5]-new_params[7]) / (1+t/new_params[8]) / sqrt(1 + t/(new_params[1]*new_params[1]*new_params[8]))  	//3rd diffusion
	) + 1;						// +1, bo nieznormalizowana g(2)

// 	double G0 = gsl_vector_get (par,0);
// 	double SP = gsl_vector_get (par,1);
// 	double T = gsl_vector_get (par,2);
// 	double t_T = gsl_vector_get (par,3);
// 	double t_D1 = gsl_vector_get (par,4);
// 	double A1 = gsl_vector_get (par,5);
// 	double t_D2 = gsl_vector_get (par,6);
// 	double A2 = gsl_vector_get (par,7);
// 	double t_D3 = gsl_vector_get (par,8);
// 	double t_bl = gsl_vector_get (par,9);
// 	double B = gsl_vector_get (par,10);

}

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
static double pochodna_par (int a, double t) {
//TODO przepisac rownania z maximy :)

	 //zm. pomocnicze dla przyspieszenia obliczen
	 double Gv = G_val (t);
	 double nawias_T = (1 + p[2]*exp(-t/p[3]) - p[2]);
	 double nawias_B = (1 + p[9]*exp(-t/p[9]) - p[9]);
	 double nawias_1 = p[5] / (1+t/p[4]) / sqrt(1 + t/(p[1]*p[1]*p[4]));
	 double nawias_2 = p[7] / (1+t/p[6]) / sqrt(1 + t/(p[1]*p[1]*p[6]));
	 double nawias_3 = (1-p[5]-p[7]) / (1+t/p[8]) / sqrt(1 + t/(p[1]*p[1]*p[8]));
	 double nawiasy_123 = nawias_1 + nawias_2 + nawias_3;

	if (a==0) return Gv/p[0];				//dN
	else if (a==1) return nawias_T * nawias_B * p[0] * (	//dSP
	 ((-p[7]-p[5]+1)*t)/(p[1]*p[1]*p[1]*p[8]*(t/p[8]+1)*(t/(p[1]*p[1]*p[8])+1)*sqrt(t/(p[1]*p[1]*p[8])+1)) +
	  (p[7]*t)/(p[1]*p[1]*p[1]*p[6]*(t/p[6]+1)*(t/(p[1]*p[1]*p[6])+1)*sqrt(t/(p[1]*p[1]*p[6])+1)) +
	  (p[5]*t)/(p[1]*p[1]*p[1]*p[4]*(t/p[4]+1)*(t/(p[1]*p[1]*p[4])+1)*sqrt(t/(p[1]*p[1]*p[4])+1)) );
	else if (a==2) return p[0]*nawiasy_123*nawias_B *	//dT
	 (exp(-t/p[3]) - 1);
	else if (a==3) return p[0]*p[2]*t*nawiasy_123*nawias_B*exp(-t/p[3])/p[3]/p[3]; //dt_T
	else if (a==4) return p[0]*nawias_T*nawias_B *		//dt_D1  //CHECK! TODO nie wiem czy dziala jak nalezy :/
			(nawias_1*t/p[4]/p[4]/(1+t/p[4]) +
			 nawias_1*t/p[4]/p[4]/2/(1+t/p[4]/p[1]/p[1])/p[1]/p[1]) ;
	else if (a==5) return p[0]*nawias_T*nawias_B *		//dA1
			(nawias_1/p[5]-nawias_3/(1-p[5]-p[7])) ;
	else if (a==6) return p[0]*nawias_T*nawias_B *		//dt_D2
			(nawias_2*t/p[6]/p[6]/(1+t/p[6]) +
			 nawias_2*t/p[6]/p[6]/2/(1+t/p[6]/p[1]/p[1])/p[1]/p[1]) ;
	else if (a==7) return p[0]*nawias_T*nawias_B * 		//dA2
			(nawias_2/p[7]*-nawias_3/(1-p[5]-p[7])) ;
	else if (a==8) return p[0]*nawias_T*nawias_B *		//dt_D3
			(nawias_3*t/p[8]/p[8]/(1+t/p[8]) +
			 nawias_3*t/p[8]/p[8]/2/(1+t/p[8]/p[1]/p[1])/p[1]/p[1]) ;
	else if (a==9) return  p[0]*p[10]*t*nawiasy_123*nawias_T*exp(-t/p[9])/p[9]/p[9]; //dt_bl
	else if (a==10) return p[0]*nawias_T*nawiasy_123*(exp(-t/p[9])-1) ; //dB
	else {std::cerr << "Invalid parameter number!"; exit (2);}
}

//Funkcja ustawiajaca macierz J wedlug zadanego rownania
//par - wektor parametrow, data - dane, J - jakobian - macierz pochodnych
// czastkowych funkcji po kolejnych parametrach dopasowywanych dla calej dziedziny funkcji
static int G_df (const gsl_vector * par, void *data, gsl_matrix * J) {
std::cout << "g_dF! ";
       uint n = ((struct data *)data)->n;
       double *sigma = ((struct data *) data)->sigma;	//skroty do danych
       double *x = ((struct data *) data)->x;

       for (uint j = 0; j < num_par; j++)
	 printf ("Pochodna (x=0)[%d]: %lf, numerycznie: %lf\n",j,pochodna_par (permut[j],x[0]), pochodna_numer (permut[j],x[0]));

       for (uint i = 0; i < n; i++)	//jedziemy po wszystkich danych
         {
           /* Jacobian matrix J(i,j) = dfi / dpj, */
           /* where fi = (Yi - yi)/sigma[i],      */
           /*       Yi = wartosc funkcji obliczona  */
           /* and the pj are the parameters (N,tau_D, etc.) */
	   for (uint j = 0; j < num_par; j++) {
		gsl_matrix_set (J, i, j, pochodna_par (permut[j],x[i]) / sigma[i]);
	   }
         }
       return GSL_SUCCESS;
}

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

//zbiorcza funkcja
int G_fdf (const gsl_vector * par_fit, void *data, gsl_vector * f, gsl_matrix * J) {
       std::cout << "FDF! ";

       G_f (par_fit, data, f);
       G_df (par_fit, data, J);

       return GSL_SUCCESS;
}


bool Analyzer :: Fit () {
  if (!CalculateAverageCorrelation())  //leave only one (average) function & check if there's anything to fit to :)
	{ printf ("Average correlation could not be calculated.\n"); return false;}

  //Fill in the permutation table
  num_par = 0;
  for (int i = 0; i < NUM_PARAMS; i++) {
	p[i] = tableParameters->item(i,0)->text().toDouble();
	if (tableParameters->item(i,0)->background() != Qt::gray) {	//jezeli mamy dopasowywac dany parametr
		par_init[num_par] = p[i];
		permut[num_par] = i;					//to wpisujemy go do tablicy permutacyjnej
		std::cout << "used: "<<i<<" ("<<num_par<<") ("<<p[permut[num_par]]<<"="<<p[i]<<") ";
		num_par++;
	}
  }

  if (num_par == 0) {fprintf (stderr, "Error: no free parameters for fit.\n"); return false;}

  //Load curve data into the arrays
  QList<QwtPlotItem*> lista = qwtPlotCF->itemList();
  if (lista.size()!=1) {printf ("Warning: more than one curve on the plot!!\n");}

  QwtPlotCurve* krzywa = (QwtPlotCurve*) lista[0]; //dopasowujemy do I krzywej z wykresu
  const uint start = fitFrom->text().toUInt();
  const uint n = fitTo->text().toUInt() - fitFrom->text().toUInt();//krzywa->dataSize(); 	//n-dlugosc traj TODO
  printf ("\nnum_par = %d, n = %d (od %d do %d)\n",num_par,n,start,fitTo->text().toUInt()); //debug
  if (n < num_par) {printf ("Too few data points (n<p), cannot fit!\n"); return false;}

  double x[n], y[n], sigma[n];
  struct data d = { n, x, y, sigma};	//d - "skrot" do danych

  for (uint i = 0; i < n; ++i) {
	x[i] = krzywa->x(start+i);
	y[i] = krzywa->y(start+i);
	// std::cout << x[i] << ", " << y[i] <<"\n";
	sigma[i] = y[i];
  }

  gsl_vector_view par = gsl_vector_view_array (par_init, num_par); //Chcemy stworzyc gsl_vector z double[] array'a (a nie ma tu konstruktora)

  gsl_multifit_function_fdf f;		//f - struct. opisujaca dopasowywana funkcje
  f.f = &G_f;
  f.df = &G_df;
  f.fdf = &G_fdf;
  f.n = n;
  f.p = num_par;
  f.params = &d;

  //ustawiamy rodzaj solvera (chyba jedyny dostepny to Levenberg-Marquard ;) )
  const gsl_multifit_fdfsolver_type * T = gsl_multifit_fdfsolver_lmsder;
  gsl_multifit_fdfsolver * s = gsl_multifit_fdfsolver_alloc (T, n, num_par); // s - nasz solver
  gsl_multifit_fdfsolver_set (s, &f, &par.vector);

  unsigned int iter = 0;
  int status;

  print_state (iter, s);   //debug
  do {
           iter++;
           status = gsl_multifit_fdfsolver_iterate (s);  //iteracja dopasowywania

           printf ("status = %s\n", gsl_strerror (status));
           print_state (iter, s);
           if (status) break;

           status = gsl_multifit_test_delta (s->dx, s->x, 1e-5, 1e-5); //testujemy, czy jestesmy w granicach (epsilon)
  } while (status == GSL_CONTINUE && iter < MAX_ITERS);

  gsl_matrix *covar = gsl_matrix_alloc (num_par, num_par);  //covar - macierz kowariancji (do obliczenia bledow)
  gsl_multifit_covar (s->J, 0.0, covar);

  #define FIT(i) gsl_vector_get(s->x, i)
  #define ERR(i) sqrt(gsl_matrix_get(covar,i,i))

  double chi = gsl_blas_dnrm2(s->f);
  double dof = n - num_par;
  double c = GSL_MAX_DBL(1, chi / sqrt(dof)); 

  printf ("chisq/dof = %g\n",  chi*chi / dof);
  for (uint i = 0; i < num_par; i++) printf ("p[%d]\t = %.5f +/- %.5f\n", permut[i], FIT(i), c*ERR(i));
  printf ("status = %s\n", gsl_strerror (status));

  //uaktualniamy nowe dopasowane wartosci (Not sure if necessary)
  for (uint i = 0; i < num_par; i++) p[permut[i]] = gsl_vector_get (s->x,i);

  //display results back in tableParameters
  for (uint i = 0; i < NUM_PARAMS; i++) tableParameters->item(i,0)->setText(QString::number(p[i]));

  gsl_multifit_fdfsolver_free (s);
  gsl_matrix_free (covar);

  DrawCorrelationFromParams();

  return true;
}
