#include "program.h"
#include <stdio.h>
Program :: Program () {
    setupUi (this);
    tabWidget->clear();
    
    connect (butLoadFCSData, SIGNAL (clicked()), this, SLOT (OpenFCSFile())); //DONE Ha-ha! :)
    connect (butLoadASCIIData, SIGNAL (clicked()), this, SLOT (OpenASCII())); //DONE Ha-ha! :)
    connect (butLoadRAW, SIGNAL (clicked()), this, SLOT (OpenRAWFile()));
    connect (tabWidget, SIGNAL (tabCloseRequested (int)), this, SLOT (RemoveTab (int)));
}

void Program :: keyPressEvent (QKeyEvent * e) {
  int k = e->key();
  int m = e->modifiers();
  int i = tabWidget->currentIndex();
  
  if (k == Qt::Key_W && m == Qt::ControlModifier) tabWidget->removeTab(i); //close tab
  else if (k == Qt::Key_PageDown && m == Qt::ControlModifier) {		//next tab
      if (tabWidget->currentIndex() == tabWidget->count()-1) tabWidget->setCurrentIndex (0);
      else tabWidget->setCurrentIndex (tabWidget->currentIndex() + 1);
  } else if (k == Qt::Key_PageUp && m == Qt::ControlModifier) {		//previous tab
      if (tabWidget->currentIndex() == 0) tabWidget->setCurrentIndex (tabWidget->count () - 1);
      else tabWidget->setCurrentIndex (tabWidget->currentIndex() - 1);
  }
}

void Program :: RemoveTab (int index) {
    tabWidget->removeTab (index);
}

void Program ::  OpenFCSFile () {
    QStringList name_list = QFileDialog :: getOpenFileNames (NULL,tr("Open FCS data file"),"~",tr("Correlation function files (*.fcs);;All files (*.*)"));
    if (name_list.size() == 0) {QMessageBox::information( NULL, "Information", "No file selected.");return;}
    
    QStringList::Iterator it = name_list.begin();
    while(it != name_list.end()) {
	Analyzer * nowy_tab = new Analyzer (this);
	nowy_tab->OpenFCSFile(*it);
	tabWidget->addTab (nowy_tab, nowy_tab->filename);
	tabWidget->setCurrentWidget (nowy_tab);
	++it;
    }
}

void Program :: OpenASCII () {
    Analyzer * nowy_tab = new Analyzer (this);
    nowy_tab->OpenASCII();
    tabWidget->addTab (nowy_tab, "ASCII");
    tabWidget->setCurrentWidget (nowy_tab);
}

void Program :: OpenRAWFile () {
    Analyzer * nowy_tab = new Analyzer (this);
    nowy_tab->OpenRAWFile();
    tabWidget->addTab (nowy_tab, "RAW");
    tabWidget->setCurrentWidget (nowy_tab);
}

void Program :: on_butLoadSimul_clicked () {
    QStringList name_list = QFileDialog :: getOpenFileNames (NULL,tr("Open simulated correlation file"),"~",tr("ASCII Data files (*.dat);;All files (*)"));
    if (name_list.size() == 0) {QMessageBox::information( NULL, "Information", "No file selected.");return;}
    
    QStringList::Iterator it = name_list.begin();
    while(it != name_list.end()) {
	Analyzer * nowy_tab = new Analyzer (this);
	nowy_tab->OpenSimulated(*it);
	tabWidget->addTab (nowy_tab, nowy_tab->filename);
	tabWidget->setCurrentWidget (nowy_tab);
	++it;
    }
}

void Program :: on_actionSaveAll_triggered () { //autoconnect actually works!!
  QString name = QFileDialog :: getSaveFileName (this,tr("Select a new output file"),tr("~"),tr("Parameter files (*.par);;All files (*.*)"));
  if (name == NULL) {QMessageBox::information( NULL, "Information", "Output file not selected.");return;}

  FILE * fp = fopen (name.toAscii(), "wt");

  fprintf (fp, "name\t\t\tG0\t\tSP\t\tT\t\tt_T\t\tt_D1\t\tA1\t\tt_D2\t\tA2\t\tt_D3\t\tt_bl\t\tB\t\tChi^2\n");

  for (int i = 0; i < tabWidget->count(); i++) { 
	Analyzer * anal = (Analyzer *) tabWidget->widget(i);
	fprintf (fp, "%s\t",tabWidget->tabText(i).toAscii().data()); 
	for (uint j = 0; j < NUM_PARAMS; j++) {
	    if (anal->tableParameters->item(j,0)->background() != Qt::gray) 
		fprintf (fp, "%le\t", anal->tableParameters->item(j,0)->text().toDouble());
	    else 
		fprintf (fp, "(%le)\t", anal->tableParameters->item(j,0)->text().toDouble());
	  }
	fprintf (fp, "%le\t",anal->lblChiSqr->text().toDouble());
	fprintf (fp, "\n");
  }

  fclose(fp);
  QMessageBox::information( NULL, "Finished", QString("All fit parameters saved to: ")+name);
}

void Program :: on_actionFitAll_triggered () {
  Analyzer * curr = (Analyzer *) tabWidget->widget(tabWidget->currentIndex ());

  stBar->showMessage ("Fitting...");  //timeout=20s  
  
  for (int i = 0; i < tabWidget->count(); i++) {
	Analyzer * anal = (Analyzer *) tabWidget->widget(i);
	for (uint p = 0; p < NUM_PARAMS; p++) {
	    anal->tableParameters->item(p,0)->setBackground (curr->tableParameters->item(p,0)->background());
	    anal->tableParameters->item(p,0)->setText(QString::number(curr->tableParameters->item(p,0)->text().toDouble()));
	}
	anal->inputBaseLine->setText (curr->inputBaseLine->text()); 	//baseline
	anal->comboModel->setCurrentIndex (curr->comboModel->currentIndex());

	if (!actionKeepSeparateFitLimits->isChecked()) {  //if we uncheck the "Keep separate ..." menu item, force all limits to the same values
	  anal->fitFrom->setText (curr->fitFrom->text());    		//granice
	  anal->fitTo->setText (curr->fitTo->text());
	}  
	anal->Fit();
  }

  stBar->showMessage ("Fitting all functions complete!", 10000);  //timeout=20s
}
