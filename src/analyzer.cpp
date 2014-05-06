#include <QVBoxLayout>
#include "analyzer.h"
#include "program.h"
#include <qwt_plot.h>
#include <qwt_symbol.h>
#include <stdio.h>

using std::cout;
using std::cerr;

void IndexedPushButton :: emit_i () {
     emit clicked(i, SM::SWITCH);
}

// -------- Oblicza srednia funkcje korelacji Z ZAZNACZONYCH ------------- //
bool Analyzer :: CalculateAverageCorrelation () {
  if (info == NULL) { cout << "No data loaded!\n"; return false;}
  if (listFunctions->count() < 1) { cout << "***ERROR in code: No data loaded & NULL not detected!!!***\n"; return false;} //should not happen!
  Info::Func srednia;                      //NOTE we no longer assume all the correlations nor countrates have the same size
  Info::Func srednia_countrate;            //NOTE instead, let's take the shortest one for the sake of averaging
  uint size = info->correlation[0].size();
  uint count_size = info->countrate[0].size(); //#points in one correlation

  for (uint f = 0; f < info->correlation.size(); f++) { //info->correlation.size - #correlations;
    if (info->correlation[f].size() < size) size = info->correlation[f].size(); //size = teraz minimalny (rowny najkrotszej f-cji korelacji)
    if (info->countrate[f].size() < count_size ) count_size = info->countrate[f].size(); //size = teraz minimalny (rowny najkrotszej f-cji korelacji)
  }
  
  srednia.resize(size); srednia_countrate.resize(count_size);

  uint checked = 0;  

  for (uint f = 0; f < info->correlation.size(); f++) { //info->correlation.size - #correlations;
    if (!listFunctions->item(f)->isSelected()) continue; //usredniamy tylko funkcje wybrane z listy!

//    if (info->correlation[f].size() != size) {cout << "Error: correlation arrays have different sizes!"; return false;}
    for (uint a = 0; a < size; a ++) srednia[a].y += info->correlation[f][a].y; //a - wspolrzedna X
//    if (info->countrate[f].size() != count_size) {cout << "Error: countrate trajectories have different sizes!"; return false;}
    for (uint a = 0; a < count_size; a ++) srednia_countrate[a].y += info->countrate[f][a].y; //a - wspolrzedna X

    checked ++;
  }

  if (checked == 0) {cerr << "No function selected.\n"; return false;} 	//nie zaznaczylismy zadnej funkcji

  for (uint a = 0; a < size; a++) {
    srednia[a].x = info->correlation[0][a].x;
    srednia[a].y /= checked;//info->correlation.size(); nie wszystkie sa zaznaczone  
  }

  for (uint a = 0; a < count_size; a ++) {
    srednia_countrate[a].x = a;
    srednia_countrate[a].y /= checked;
  }
  
  qwtPlotCF->detachItems (QwtPlotItem::Rtti_PlotCurve); //skasuj wszystkie krzywe z wykresu
  DrawFunction (srednia, "Average", qwtPlotCF); //i namaluj tylko srednia

  qwtPlotResiduals->detachItems (QwtPlotItem::Rtti_PlotCurve); //wyczysc okno i namaluj sredni przebieg intensywnosci
  DrawFunction (srednia_countrate, "Average countrate", qwtPlotResiduals);

  return true; //return true if anything is drawn
}

void DrawFunction (const Info::Func & f, QString text, QwtPlot * plot) {
  //display countrate in the top graph for the first experiment
  int ar_size = f.size(); //array size

  double * x = new double [ar_size];	//copy the values from FCS_Info->vector into a simple array
  double * y = new double [ar_size];
  for (int i=0; i<ar_size; ++i) {
	x[i] = f[i].x;
	y[i] = f[i].y;
  }

  QwtPlotCurve * curve = new QwtPlotCurve (text);
  curve->setSamples (x,y,ar_size);
  curve->attach (plot);
  plot->replot();

  delete [] x; delete [] y;
}

// --------- Otworz plik z danymi, namaluj je na wykresie i dodaj do okienka funkcji ---------- //
void Analyzer :: OpenFCSFile (QString fname) {
  if (info != NULL) delete info;
  info = new Info::FCS_Info;

  if (!info->LoadFCSFile(fname)) 
	{QMessageBox::warning( NULL, "Error", tr("Failed to load file %1.").arg(fname)); return;}

  info->Print (std::cout); /////DEBUG

  listFunctions->clear();          //wyczysc okienko z lista funckji
  qwtPlotCF->detachItems (QwtPlotItem::Rtti_PlotCurve); //skasuj wszystkie krzywe z wykresu
  qwtPlotResiduals->detachItems (QwtPlotItem::Rtti_PlotCurve); //skasuj wszystkie krzywe z wykresu

  for (int i=0;i<info->num_repeats();i++) {   //dodaj funkcje korelacji do okienka
      QListWidgetItem * item = new QListWidgetItem;
      item->setText(tr("Function %1: %2").arg(i+1).arg(info->AvgIntensity(i)));
      listFunctions->addItem (item);

      DrawFunction (info->countrate[i], tr("Intensity #%1").arg(i), qwtPlotResiduals); //namaluj funkcje na wykresie
      DrawFunction (info->correlation[i], tr("Correlation #%1").arg(i), qwtPlotCF);
  }

  fitFrom->setText (QString::number(0));
  fitTo->setText (QString::number(info->correlation[0].size()));

  if (fname.length() > 25) filename = QString ("...") + fname.right(25);
  else filename = fname;
  
  listFunctions->setCurrentRow(0);
  
  //Ustaw tytul, zeby bylo widac nazwe pliku
  /*MainWindow->*///setWindowTitle (QString("FCS analyzer: ")+name);
/*  QTabWidget * p = (QTabWidget *) parent();
  p->setTabText (p->currentIndex(), name);*/
}

// ------------------- Otwórz plik wysymulowany - format ASCII, z pominieciem I linijki ------------ //
void Analyzer :: OpenSimulated (QString fname) {
    if (info != NULL) delete info;
    info = new Info::FCS_Info;

    info->correlation.resize(1); info->repeats = 1;
    info->countrate.resize(1); info->countrate[0].resize(1); //wlasciwie po nic, ale niech bedzie,zeby nie segfaulcil HACK
       
    QFile fp (fname);
    if (!fp.open(QIODevice::ReadOnly | QIODevice::Text)) {  QMessageBox::warning( NULL, "Error", "Could not open file."); return; }
      
    QString line;
    line = fp.readLine().trimmed(); //skip 1st line  
    
    while ((line = fp.readLine().trimmed())!=NULL) {
	    QStringList words = line.split(QRegExp("\\s+"),QString::SkipEmptyParts);

	    Info :: XYData xydata;
	    xydata.x = words[0].toDouble();
	    xydata.y = words[1].toDouble();
	    info->correlation[0].push_back(xydata);
    }
      
    fp.close();

    info->Print (std::cout); /////DEBUG

    listFunctions->clear();          //wyczysc okienko z lista funckji
    qwtPlotCF->detachItems (QwtPlotItem::Rtti_PlotCurve); //skasuj wszystkie krzywe z wykresu
    qwtPlotResiduals->detachItems (QwtPlotItem::Rtti_PlotCurve); //skasuj wszystkie krzywe z wykresu

    QListWidgetItem * item = new QListWidgetItem;
    item->setText(tr("%1").arg(fname));
    listFunctions->addItem (item);

    //DrawFunction (info->countrate[0], tr("Intensity #%1").arg(i), qwtPlotResiduals); //namaluj funkcje na wykresie
    DrawFunction (info->correlation[0], tr("Simulated Correlation"), qwtPlotCF);
    
    fitFrom->setText (QString::number(0));
    fitTo->setText (QString::number(info->correlation[0].size()));

    listFunctions->setCurrentRow(0);

    inputBaseLine->setText("0");
    if (fname.length() > 25) filename = QString ("...") + fname.right(25);
    else filename = fname;
    
    //Ustaw tytul, zeby bylo widac nazwe pliku TODO
    setWindowTitle (QString("FCS analyzer: Simulated data"));
}

// ------------------- Otwórz plik z danymi w formacie ASCII ------------ //
void Analyzer :: OpenASCII () {
    printf ("Opening ASCII file...");

    QStringList name_list = QFileDialog :: getOpenFileNames (NULL,tr("Open ASCII data file"),"~",tr("ASCII Data files (*.dat);;All files (*)"));
    if (name_list.size() == 0) {QMessageBox::information( NULL, "Information", "No file selected.");return;}
    
    if (info != NULL) delete info;
    info = new Info::FCS_Info;

//       if (!info->LoadFCSFile(name)) 
// 	  {QMessageBox::warning( NULL, "Error", tr("Failed to load file %1.").arg(name)); return;}

    info->correlation.resize(name_list.size());
    info->countrate.resize(name_list.size());	//wlasciwie po nic, ale niech bedzie,zeby nie segfaulcil HACK

    info->repeats = name_list.size();
    
    for (int c = 0; c < name_list.size(); ++c) {	//c - #funkcji korelacji
      QFile fp (name_list.at(c));
      if (!fp.open(QIODevice::ReadOnly | QIODevice::Text)) {  QMessageBox::warning( NULL, "Error", "Could not open file."); return; }
      
      QString line;
      
      while ((line = fp.readLine().trimmed())!=NULL) {
	    QStringList words = line.split(QRegExp("\\s+"),QString::SkipEmptyParts);

	    Info :: XYData xydata;
	    xydata.x = words[0].toDouble();
    	    xydata.y = words[1].toDouble();

	    info->correlation[c].push_back(xydata);
      }
      
      fp.close();
      info->countrate[c].resize(1);
    }

    info->Print (std::cout); /////DEBUG

    listFunctions->clear();          //wyczysc okienko z lista funckji
    qwtPlotCF->detachItems (QwtPlotItem::Rtti_PlotCurve); //skasuj wszystkie krzywe z wykresu
    qwtPlotResiduals->detachItems (QwtPlotItem::Rtti_PlotCurve); //skasuj wszystkie krzywe z wykresu

    for (int i=0;i<info->num_repeats();i++) {   //dodaj funkcje korelacji do okienka
	QListWidgetItem * item = new QListWidgetItem;
	item->setText(tr("%1").arg(name_list[i]));
	listFunctions->addItem (item);

	DrawFunction (info->countrate[i], tr("Intensity #%1").arg(i), qwtPlotResiduals); //namaluj funkcje na wykresie
	DrawFunction (info->correlation[i], tr("Correlation #%1").arg(i), qwtPlotCF);
    }

    fitFrom->setText (QString::number(0));
    fitTo->setText (QString::number(info->correlation[0].size()));

    listFunctions->setCurrentRow(0);
    
    //Ustaw tytul, zeby bylo widac nazwe pliku TODO
    setWindowTitle (QString("FCS analyzer: ASCII data"));
}

void Analyzer :: DrawCorrelationFromParams () {
  const uint N = qwtPlotCF->transform(QwtPlot::xBottom,qwtPlotCF->axisScaleDiv(QwtPlot::xBottom)->upperBound());
  double x[N],y[N];

  double par [NUM_PARAMS];
  for (uint i = 0; i < NUM_PARAMS; i++) par[i] = tableParameters->item(i,0)->text().toDouble();

  for (uint i = 0; i < N; i++) {
	x[i] = qwtPlotCF->invTransform(QwtPlot::xBottom,i);
	y[i] = G_val (x[i],par);
// 	std::cout << x[i] << ", " << y[i] <<"\n";
  }

  QwtPlotCurve * curve = new QwtPlotCurve ("G(tau) - fitted");
  QPen kolor (Qt::red);
  curve->setSamples (x,y,N);
  curve->setPen (kolor);
  curve->attach (qwtPlotCF);
//   qwtPlotCF->setAxisAutoScale (xBottom);
  qwtPlotCF->setAxisScaleDiv (QwtPlot::xBottom, *qwtPlotCF->axisScaleDiv(QwtPlot::xBottom));//HACK wylacza autoscaling :)
  qwtPlotCF->replot();

  // ------------ Draw residuals in the upper plot ---------------//
  if (info == NULL) return; //do not draw residuals if there is no correlation loaded
  
  qwtPlotResiduals->detachItems (QwtPlotItem::Rtti_PlotCurve); //clean up the upper plot

  // Oblicz srednia (jak w CalculateAverageCorrelation)
  Info::Func funkcja;                      //NOTE assume all the correlations have the same size
  uint size = fitTo->text().toUInt() - fitFrom->text().toUInt() + 1; //#points in one correlation
  funkcja.resize(size);
  uint checked = 0;  

  for (uint f = 0; f < info->correlation.size(); f++) {		//info->correlation.size - #correlations;
    if (!listFunctions->item(f)->isSelected()) continue;	//usredniamy tylko funkcje wybrane z listy!
    for (uint a = 0; a < size; a ++) funkcja[a].y += info->correlation[f][a + fitFrom->text().toUInt()].y; //a - wspolrzedna X
    checked ++;
  }

  for (uint a = 0; a < size; a++) {
    funkcja[a].x = info->correlation[0][a + fitFrom->text().toUInt()].x;
    funkcja[a].y /= checked; //dzielimy sume przez liczbe funkcji (nie wszystkie sa zaznaczone) -> dostajemy srednia z wynikow
    funkcja[a].y = funkcja[a].y - G_val (funkcja[a].x,par); //odejmujemy od niej wartosc dopasowana
  }
  
  QwtScaleEngine * log_engine = new QwtLog10ScaleEngine;
  qwtPlotResiduals->setAxisScaleEngine (QwtPlot::xBottom, log_engine); //ustaw skale na logarytmiczna
  qwtPlotResiduals->setAxisScaleDiv (QwtPlot::xBottom, *qwtPlotCF->axisScaleDiv(QwtPlot::xBottom));//HACK manual scale - z dolnego grafu:)

  DrawFunction (funkcja, "Average", qwtPlotResiduals); //i namaluj tylko srednia

//   qwtPlotResiduals->replot();
}

void Analyzer :: setFitLimits(int button, double x, double) { //double y- niewykorzystane
  if (button == Qt::MidButton) { //set left point
	uint i;
	for (i = 0; i < info->correlation[0].size(); ++i) {
		if (info->correlation[0][i].x > x) break;
	}
	if (i >= fitTo->text().toUInt()) i = fitTo->text().toUInt() - 1;
	fitFrom->setText (QString::number(i));
	labelFrom->setText (QString("(")+QString::number(info->correlation[0][i].x)+") to ");

	QList<QwtPlotItem*> lista = qwtPlotCF->itemList();
	QwtPlotCurve * krzywa = (QwtPlotCurve *) lista[0];
	
	m1.setValue (krzywa->sample(i).x(), krzywa->sample(i).y());
	m1.attach (qwtPlotCF);
  }
  else if (button == Qt::RightButton) { //set right point
	uint i;
	for (i = 0; i < info->correlation[0].size(); ++i) {
		if (info->correlation[0][i].x > x) break;
	}
	if (i == info->correlation[0].size()) //jezeli przelecielismy cala petle i nadal x jest za maly
	    i = info->correlation[0].size() - 1; //ustaw marker na ostatnim punkcie krzywej
	
	if (i <= fitFrom->text().toUInt()) i = fitFrom->text().toUInt() + 1;
	fitTo->setText (QString::number(i));
	labelTo->setText (QString("(")+QString::number(info->correlation[0][i].x)+") seconds");

	QList<QwtPlotItem*> lista = qwtPlotCF->itemList();
	QwtPlotCurve * krzywa = (QwtPlotCurve *) lista[0];
	
	m2.setValue (krzywa->sample(i).x(), krzywa->sample(i).y());
	m2.attach (qwtPlotCF);
	cout << "Marker set at "<<m2.xValue() << ", "<<m2.yValue()<<"\n"; cout.flush();
  }

  qwtPlotCF->replot();
}

void Analyzer :: setFitModel (int model) {
	if (model == 1) {	//1-component
		tableParameters->lockCell (0,SM::ON);	//odblokuj tylko G(0)
		tableParameters->lockCell (4,SM::ON);	//i t_D1

		tableParameters->lockCell (5,SM::OFF);	
		tableParameters->lockCell (6,SM::OFF);	
		tableParameters->lockCell (7,SM::OFF);	
		tableParameters->lockCell (8,SM::OFF);	

		tableParameters->setValue(1.0, 5,0); //A1 = 1
		tableParameters->setValue(0.0, 7,0); //A2 =~ 0
	}
	if (model == 2) {	//2-component
// 		for (int i=0;i<NUM_PARAMS;i++) tableParameters->lockCell (i,SM::OFF);
		tableParameters->lockCell (0,SM::ON);	//odblokuj tylko G(0),
		tableParameters->lockCell (4,SM::ON);	//t_D1
		tableParameters->lockCell (5,SM::ON);	//A1
		tableParameters->lockCell (8,SM::ON);	//i t_D3

		tableParameters->lockCell (6,SM::OFF);	
		tableParameters->lockCell (7,SM::OFF);	

		tableParameters->setValue(0.5, 5,0); //A1 = 1  //wystarczy, ze to sie zmieni: A3 = 1-A1-A2 = 1-A1-0 = 1-A1 = 0.5 na starcie
		tableParameters->setValue(0.0, 7,0); //A2 =~ 0
	}
	if (model == 3) {	//3-component
// 		for (int i=0;i<NUM_PARAMS;i++) tableParameters->lockCell (i,SM::OFF);
		tableParameters->lockCell (0,SM::ON);	//odblokuj G(0),
		tableParameters->lockCell (4,SM::ON);	//t_D1
		tableParameters->lockCell (5,SM::ON);	//A1
		tableParameters->lockCell (6,SM::ON);	//t_D2
		tableParameters->lockCell (7,SM::ON);	//A2
		tableParameters->lockCell (8,SM::ON);	//t_D3

		tableParameters->setValue(0.33, 5,0); //A1 = 0.33
		tableParameters->setValue(0.33, 7,0); //A2 = 0.33
	}
}

void Analyzer :: setTripletModel (int s) {
	if (s == Qt::Checked) {
		tableParameters->setValue(0.2, 2,0);  //T = 0.2
		tableParameters->setValue(1, 3,0);  //t_T = 10
		tableParameters->lockCell (2,SM::ON); //odblokuj T
		tableParameters->lockCell (3,SM::ON); //odblokuj t_T
	}
	else {
		tableParameters->setValue(0.0, 2,0);  //T =~ 0
		tableParameters->lockCell (2,SM::OFF); //zablokuj T
		tableParameters->lockCell (3,SM::OFF); //zablokuj t_T
	}
}

void Analyzer :: setBleachModel (int s) {
	if (s == Qt::Checked) {
		tableParameters->setValue(0.2, 10,0);  //BT = 0.2
		tableParameters->setValue(50, 9,0);  //t_bl = 10000
		tableParameters->lockCell (10,SM::ON); //odblokuj B
		tableParameters->lockCell (9,SM::ON); //odblokuj t_bl
	}
	else {
		tableParameters->setValue(0.0, 10,0); //B =~ 0
		tableParameters->lockCell (10,SM::OFF); //zablokuj B
		tableParameters->lockCell (9,SM::OFF); //zablokuj t_bl
	}
}

void Analyzer :: SaveParams () {
  if (info == NULL) { cout << "No data loaded!\n"; return;}

  QString name = QFileDialog :: getSaveFileName (this,tr("Select file to append results to"),tr("~"),tr("Parameter files (*.par);;All files (*)"), NULL, QFileDialog::DontConfirmOverwrite);
  if (name == NULL) {QMessageBox::information( NULL, "Information", "Output file not selected.");return;}

  FILE * fp = fopen (name.toAscii(), "at");

  fprintf (fp, "name\t\t\tG0\t\tSP\t\tT\t\tt_T\t\tt_D1\t\tA1\t\tt_D2\t\tA2\t\tt_D3\t\tt_bl\t\tB\t\tChi^2\n");
  fprintf (fp, "%s\t",filename.toAscii().data()); 
  for (uint i = 0; i < NUM_PARAMS; i++) {
      if (tableParameters->item(i,0)->background() != Qt::gray) 
	fprintf (fp, "%le\t", tableParameters->item(i,0)->text().toDouble());
      else 
	fprintf (fp, "(%le)\t", tableParameters->item(i,0)->text().toDouble());
  }
  fprintf (fp, "%le\t",lblChiSqr->text().toDouble());
  fprintf (fp, "\n");

  fclose(fp);

  QMessageBox::information( NULL, "Finished", QString("Fit parameters appended to: ")+name);
}

Analyzer :: Analyzer (QWidget * par) {
  setParent (par);
  info = NULL;
  setupUi (this);

  QwtSymbol * s1 = new QwtSymbol(QwtSymbol::Diamond, QBrush(QColor(Qt::red)), QPen (QColor(Qt::black)), QSize (10,10));
  QwtSymbol * s2 = new QwtSymbol(QwtSymbol::Diamond, QBrush(QColor(Qt::green)), QPen (QColor(Qt::black)), QSize (10,10));
  
  m1.setSymbol (s1);
  m2.setSymbol (s2);

  QwtScaleEngine * log_engine = new QwtLog10ScaleEngine;
  qwtPlotCF->setAxisScaleEngine (QwtPlot::xBottom, log_engine);

  sliderN->SetExpParams (labN_min->text().toDouble(), labN_max->text().toDouble(),0,0);
  sliderT->SetExpParams (labT_min->text().toDouble(), labT_max->text().toDouble(),2,0);  
  sliderTau1->SetExpParams (labTau1_min->text().toDouble(), labTau1_max->text().toDouble(),4,0);

  connect (sliderN, SIGNAL(sendExp(double,int,int)), tableParameters, SLOT(setValue(double,int,int)));
  connect (sliderT, SIGNAL(sendExp(double,int,int)), tableParameters, SLOT(setValue(double,int,int)));
  connect (sliderTau1, SIGNAL(sendExp(double,int,int)), tableParameters, SLOT(setValue(double,int,int)));

  connect (tableParameters, SIGNAL (cellValueSet(double,int,int)), sliderN, SLOT(updateSlider(double,int,int)));
  connect (tableParameters, SIGNAL (cellValueSet(double,int,int)), sliderT, SLOT(updateSlider(double,int,int)));  
  connect (tableParameters, SIGNAL (cellValueSet(double,int,int)), sliderTau1, SLOT(updateSlider(double,int,int)));

  connect (butSelectAll, SIGNAL (clicked()), listFunctions, SLOT (selectAll()));
  connect (butSelectNone, SIGNAL (clicked()), listFunctions, SLOT (clearSelection()));
//   connect (butAverage, SIGNAL (clicked()), this, SLOT (CalculateAverageCorrelation()));
  connect (butSaveASCII, SIGNAL (clicked()), this, SLOT (SaveASCII()));
  connect (butSaveAvg, SIGNAL (clicked()), this, SLOT (SaveAvg()));
  connect (butSaveParameters, SIGNAL (clicked()), this, SLOT (SaveParams()));
  connect (butShowInfo, SIGNAL (clicked()), this, SLOT (ShowInfo()));
  
  connect (listFunctions,SIGNAL (itemSelectionChanged()), this, SLOT(CalculateAverageCorrelation()));
  connect (butFit, SIGNAL (clicked()), this, SLOT(Fit()));
  connect (butDraw, SIGNAL(clicked()), this, SLOT(DrawCorrelationFromParams()));

  connect (qwtPlotCF, SIGNAL (clicked (int , double, double)), this, SLOT(setFitLimits(int , double, double)));

  connect (comboModel, SIGNAL (currentIndexChanged (int)), this, SLOT (setFitModel (int)));
  connect (chkTriplet, SIGNAL (stateChanged (int)), this, SLOT (setTripletModel (int)));
  connect (chkPhotobleaching, SIGNAL (stateChanged (int)), this, SLOT (setBleachModel (int)));

  for (int i=0;i<NUM_PARAMS;i++) tableParameters->lockCell (i,SM::OFF); //lock all cells

  QVBoxLayout * layoutLockButtons = new QVBoxLayout; //tworzymy recznie layout,bo pod linuxem inaczej nie styka :/
  IndexedPushButton * lockbutton[NUM_PARAMS];
  for (int i=0; i<NUM_PARAMS; i++) {
    lockbutton[i] = new IndexedPushButton ("Lock", i);
    layoutLockButtons->addWidget (lockbutton[i]);
    connect (lockbutton[i], SIGNAL (clicked(int, SM::SwitchModes)), tableParameters, SLOT(lockCell(int,SM::SwitchModes)));
  }
  frameLock->setLayout(layoutLockButtons);
}

void Analyzer :: ShowInfo () {
    if (info == NULL) {QMessageBox::warning( NULL, "Error", "No file loaded."); return;}
    
    QString text = info->GetMeasurementInfo();
    QMessageBox::information( NULL, "Measurement info", text); return;
}

void Analyzer :: SaveASCII () {
  if (info == NULL) { cout << "No data loaded!\n"; return;}

  QString name = QFileDialog :: getExistingDirectory (this,tr("Select output directory"),"~",QFileDialog::ShowDirsOnly);
  if (name == NULL) {QMessageBox::information( NULL, "Information", "Output directory not selected.");return;}

  for (uint f = 0; f < info->correlation.size(); f++) { //info->correlation.size - #correlations;
	if (!listFunctions->item(f)->isSelected()) continue; //wypisujemy //NIE// tylko funkcje wybrane z listy!

	QString filename = name + "/" + QString::number(f+1) + ".dat";
	FILE * fp = fopen (filename.toLatin1(), "wt");
	 for (uint a = 0; a < info->correlation[f].size(); a ++) fprintf (fp, "%lf\t%lf\n", info->correlation[f][a].x, info->correlation[f][a].y);
	fclose(fp);
  }

  QMessageBox::information( NULL, "Finished", QString("Files saved in: ")+name);
}

void Analyzer :: SaveAvg () {
  if (info == NULL) { cout << "No data loaded!\n"; return;}

  QString name = QFileDialog :: getSaveFileName (this,tr("Select output file"),"~");
  if (name == NULL) {QMessageBox::information( NULL, "Information", "Output file not selected.");return;}

  if (!CalculateAverageCorrelation())  //leave only one (average) function & check if there's anything to fit to :)
	{ printf ("Average correlation could not be calculated.\n"); return;}

  QList<QwtPlotItem*> lista = qwtPlotCF->itemList();
  if (lista.size()!=1) {printf ("Warning: more than one curve on the plot!!\n");}

  QwtPlotCurve* krzywa = (QwtPlotCurve*) lista[0]; //dopasowujemy do I krzywej z wykresu
//   const uint start = fitFrom->text().toUInt();
//   const uint n = fitTo->text().toUInt() - fitFrom->text().toUInt();//krzywa->dataSize(); 	//n-dlugosc traj TODO

  FILE * fp = fopen (name.toAscii(), "wt");

  for (uint i = 0; i < info->correlation[0].size(); i++) { 
    fprintf (fp, "%lf\t%lf\n", krzywa->sample(i).x(), krzywa->sample(i).y());
  }

  fclose(fp);

  QMessageBox::information( NULL, "Finished", QString("Average correlation saved in: ")+name);
}

Analyzer :: ~Analyzer () {
  if (info != NULL) delete info;
}
