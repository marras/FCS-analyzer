#ifndef _MW_PROGRAM_H_
#define _MW_PROGRAM_H_

#include "ui_Main_analyzer.h"
#include "analyzer.h"
#include <QTabWidget>
#include <QMessageBox>
#include <QFileDialog>
#include <QWidget>
//#include <qwt_plot.h>
// #include <qwt_plot_curve.h>
// #include <qwt_scale_engine.h>
// #include <qwt_plot_marker.h>
// #include "tableparams.h"
#include "fcs_loader.h"

class Program : public QMainWindow, public Ui::MainWindow { //could be Ui_MainWindow as well
  Q_OBJECT 

  private:

  private slots:
     void OpenFCSFile ();
     void OpenASCII ();
     void OpenRAWFile ();
     void RemoveTab (int index);
     
     void on_actionSaveAll_triggered ();
     void on_actionFitAll_triggered ();
     void on_butLoadSimul_clicked ();
     
  protected:
     void keyPressEvent (QKeyEvent * e);
      
  public:
     Program ();
	
//  public slots:
};

#endif //_MW_PROGRAM_H_
