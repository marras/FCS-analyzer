#include "raw_loader.h"
#include "fcs_loader.h"
#include "correlator.h"


/* --- Fragment odpalajacy okno dialogowe do ladowania funkcji z RAW ----*/
void Analyzer :: OpenRAWFile () {
  //First, clean up
  listFunctions->clear();          //wyczysc okienko z lista funckji
  if (info != NULL) delete info;

  // *** NEW *** //
  RawLoader * raw_loader = new RawLoader;
  raw_loader->exec(); //NOTE: if successful, this dialog will also allocate mem for FCS_Info structure (and load it)

  //jesli kliknelismy FINISH, zamykamy dialog i pakujemy funkcje korelacji do glownego okienka
  if (raw_loader->result() == QDialog::Accepted) info = raw_loader->Wynik(); 
  
  if (info == NULL) {QMessageBox::information( NULL, "Information", "No file loaded.");return;}

  delete raw_loader;

 for (int i=0;i<info->num_repeats();i++) {   //dodaj funkcje korelacji do okienka
      QListWidgetItem * item = new QListWidgetItem;
      item->setText(tr("Function %1: %2").arg(i+1).arg(info->AvgIntensity(i)));
      listFunctions->addItem (item);

//       DrawFunction (info->countrate[i], tr("Intensity #%1").arg(i), qwtPlotIntensity); //namaluj funkcje na wykresie
//       DrawFunction (info->correlation[i], tr("Correlation #%1").arg(i), qwtPlotCF_2);
  }

//   if (name.length() > 25) filename = QString ("...") + name.right(25);
//   else filename = name;

  fitFrom->setText (QString::number(0));
  fitTo->setText (QString::number(info->correlation[0].size()));//*/
}

RawLoader ::  RawLoader () : info (NULL) {
    setupUi (this);
    connect (butLoadRAW, SIGNAL (clicked()), this, SLOT (OpenRAWFile()));
    connect (butCalculateIntensity, SIGNAL (clicked()), this, SLOT (CalculateIntensity()));
    connect (butCalculateCorrelation, SIGNAL (clicked()), this, SLOT (CalculateRAWCorrelation()));
    connect (butFinish, SIGNAL (clicked()), this, SLOT (accept()));
    connect (butCalcCorrRightAway, SIGNAL (clicked()), this, SLOT (CalcCorrRightAway()));
    
    sliderDT->SetExpParams (labDT_min->text().toDouble(), labDT_max->text().toDouble(),0,0);		/////// testing
    connect (sliderDT, SIGNAL(sendExp(double,int,int)), inputDT, SLOT(setValue(double,int,int)));		/////// testing
    connect (inputDT, SIGNAL(cellValueSet(double,int,int)), sliderDT, SLOT (updateSlider(double,int,int)));/////// testing
    
    QwtScaleEngine * log_engine = new QwtLog10ScaleEngine;
    qwtPlotCF_2->setAxisScaleEngine (QwtPlot::xBottom, log_engine);
};

/* ------  Glowna funkcja ladujaca plik RAW --------- */
void RawLoader :: OpenRAWFile() {
  QString name = QFileDialog :: getOpenFileName (NULL,tr("Open RAW data file"),"~",tr("RAW intensity trajectory files (*.raw);;All files (*.*)"));
  if (name == NULL) {/*QMessageBox::information( NULL, "Information", "No file selected.");*/return;}

  qwtPlotCF_2->detachItems (QwtPlotItem::Rtti_PlotCurve); //skasuj wszystkie krzywe z wykresu CF_2 (w II tabie)

 //check if file's ok
  QFile fp (name);
  if (!fp.open(QIODevice::ReadOnly)) {
	 QMessageBox::warning( NULL, "Error", "Could not open file.");
	 return;
  }
  
  QByteArray qba = fp.readAll();
  const char * data = qba.constData(); //wskaznik do kolejnych bajtow wczytanych z pliku

  
  
//  if (qba.size() % 2) QMessageBox::warning( NULL, "Error", "Incorrect RAW file length (odd number of bytes)!");
 ch1.clear(); ch2.clear(); //wyczyscmy dane z obu kanalow

 int ch1_cycles = 1, ch2_cycles = 1; //ile czasu minelo od ostatniego zliczenia :) (czyli "odstep"+1 kanal)

 for (int i=0; i<qba.size()/2; i++) { 	//2 bytes = sizeof(short)
	short word = ((short*)data)[i];

	uchar time_elapsed = (uchar) (word & 255);	//low byte
	uchar pulses_code = (uchar) ((word >> 8) & 255);  //high byte

// 	printf ("Time elapsed: %d\n",(int)time_elapsed);

	ch1_cycles += time_elapsed; ch2_cycles += time_elapsed;

	if (pulses_code == 0) {
		ch1_cycles += 4; ch2_cycles += 4;
		continue; //brak zliczen - jedziemy dalej
	}

// 	printf ("ch1_cycles: %d\n",ch1_cycles);

	for (int bt=0; bt<4; bt++) {
		char kod = pulses_code & 3;

		if (kod != 0) {
			if (kod & 1) {ch1.push_back (ch1_cycles); ch1_cycles = 0;} 
			if (kod & 2) {ch2.push_back (ch2_cycles); ch2_cycles = 0;}
		}

		ch1_cycles ++; ch2_cycles ++;
		pulses_code >>= 2;
	}
 }

 LOG("Channel1: %d photons, channel 2: %d photons.",ch1.size(),ch2.size());

 setWindowTitle (QString("RAW analyzer: ")+name); //Ustaw tytul, zeby bylo widac nazwe pliku
 inputRAWFileName->setText(name);		  //Ustaw nazwe pliku w polu edycji
 butCalculateIntensity->setEnabled(true);
}

using namespace Info;
using std::cout;

/* ------  Oblicz funkcje korelacji bez kanalizowania w traj. int. (oszcz. pamieci)--------- */
void RawLoader :: CalcCorrRightAway () {
 if (info!= NULL) {
   info->correlation.clear();
   info->correlation.resize(1);	//clean up first
 }
 else {
   info = new FCS_Info; 
 }
 
 //obliczamy korelacje
 info->repeats = 1;
 const double dT = inputDT->text().toInt();
 const int STEPS_PER_BIN = inputDT_2->text().toInt();

 vector<int> & channel = radioCh1->isChecked() ? ch1 : ch2;
 
 ///z main.cpp (fluor4)----------------------
 int counter = 0; //counter=1;
 double time = STEPS_PER_BIN*dT; //chcialoby sie dac "0", ale to brzydko wyglada
 double TIME_STEP = STEPS_PER_BIN;
 double TAU_MAX;// = ((double)info->countrate[0].size())*dT*0.1; //////////

 CorrelatorPAT * corr1 = new CorrelatorPAT (channel);//, dT, info->countrate[0].size(), STEPS_PER_BIN);
 
 printf ("Calculating LOG correlation (DIRECTLY from RAW) for BIN_SIZE = %d; dT = %lf, TAU_MAX = %lf.\n",STEPS_PER_BIN, dT, TAU_MAX);
 while (time <= TAU_MAX) {
      double result;// = corr1->Calculate_correlation (time); NOTE odblokowac
      printf ("g(%5.9le)=%lf\n", time*50e-9, result);

      XYData xy_data;
      xy_data.x = time*50e-9;
      xy_data.y = result;
      info->correlation[0].push_back(xy_data);

      counter++;
      if (counter >= 4) {counter=0; TIME_STEP*=2;}	//every 4 iterations, the correlation time increase is doubled
      time += TIME_STEP*dT;
 }

///---------------------
 DrawFunction (info->correlation[0], "RAW-based direct correlation", qwtPlotCF_2);
 butFinish->setEnabled(true);
 
 delete corr1;
}





/* ------  Pogrupuj zliczenia w kanaly i oblicz intensywnosc --------- */
void RawLoader :: CalculateIntensity () {
 qwtPlotCF_2->detachItems (QwtPlotItem::Rtti_PlotCurve); //skasuj wszystkie krzywe z wykresu CF_2 (w II tabie)
 qwtPlotIntensity->detachItems (QwtPlotItem::Rtti_PlotCurve); //skasuj wszystkie krzywe z wykresu Intensywnosci

 //mamy juz czasy odstepu w ch1 i ch2 - teraz nalezy przerobic na intensywnosc wg zadanego kanalu.
 vector<int> & channel = radioCh1->isChecked() ? ch1 : ch2;

 uint B = inputDT->text().toUInt();//szerokosc kanalu [FIXME na razie w cyklach]
 labelBinSize->setText(QString("= ")+QString::number(B*0.05)+QString(" [us]."));
 
 uint counts = 0, gap = 0; //gap - odstep miedzy zliczeniami

 info = new Info::FCS_Info;
 info->countrate.clear();
 info->countrate.resize (1);	//just one intensity trajectory

 for (uint a=0; a < channel.size(); a++) {
	gap += channel[a];

	counts += 1;

	while (gap > B) {
		gap -= B;
// 		printf ("Next bin! remaining gap = %d, counts = %d.\n",gap,counts);

		Info::XYData xy_data;
		xy_data.x = info->countrate[0].size();	//TODO  //Clock rate: 20 MHz = 50 ns 
		xy_data.y = counts;
		info->countrate[0].push_back(xy_data);
		counts = 0;
	}
 }

 DrawFunction (info->countrate[0], "RAW intensity", qwtPlotIntensity);
 butCalculateCorrelation->setEnabled(true); 
}

// TODO FIXME PROBLEM: Trajektoria intensywnosci dla malego kanalu nie miesci sie w pamieci!!!!!!
// Moze zrobic korelacje jednak na podstawie channeli???? JAK? TODO znalezc publikacje o HW correlatorach


void RawLoader :: CalculateRAWCorrelation () {
 info->correlation.clear();
 info->correlation.resize(1);	//clean up first

 //obliczamy korelacje
 int * traj = new int [info->countrate[0].size()];
 for (uint i=0; i<info->countrate[0].size(); i++) traj[i] = info->countrate[0][i].y;
 info->repeats = 1;

 const double dT = inputDT->text().toInt();
 const int STEPS_PER_BIN = inputDT_2->text().toInt();

 Correlator * corr1 = new CorrelatorSimple (traj, dT, info->countrate[0].size(), STEPS_PER_BIN);

	  //z main.cpp (fluor4)
          int counter = 0; //counter=1;
	  double time = STEPS_PER_BIN*dT; //chcialoby sie dac "0", ale to brzydko wyglada
	  double TIME_STEP = STEPS_PER_BIN;

	  double TAU_MAX = ((double)info->countrate[0].size())*dT*0.1;

	  printf ("Calculating LOG correlation for BIN_SIZE = %d; dT = %lf, TAU_MAX = %lf.\n",STEPS_PER_BIN, dT, TAU_MAX);

// 	  fp = fopen ("correlationLOG.dat","wt");

	  while (time <= TAU_MAX) {
	      double result = corr1->Calculate_correlation (time);
	      printf ("g(%5.9le)=%lf\n", time*50e-9, result);

	   XYData xy_data;
	   xy_data.x = time*50e-9;
	   xy_data.y = result;
	   info->correlation[0].push_back(xy_data);

	      counter++;
	      if (counter >= 4) {counter=0; TIME_STEP*=2;}	//every 4 iterations, the correlation time increase is doubled
	      time += TIME_STEP*dT;
	}

//   info->Print (std::cout); /////DEBUG


  DrawFunction (info->correlation[0], "RAW-based correlation", qwtPlotCF_2);
  butFinish->setEnabled(true);
  
  delete [] traj;
  delete corr1;
}
