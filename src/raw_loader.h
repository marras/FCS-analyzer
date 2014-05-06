#ifndef _MW_RAW_LOADER_
#define _MW_RAW_LOADER_

#include "ui_Raw_loader.h"
#include "analyzer.h"
#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>
//#include <qwt_plot.h>
// #include <qwt_plot_curve.h>
#include <qwt_scale_engine.h>
#include <qwt_plot_marker.h>
// #include "tableparams.h"
#include "fcs_loader.h"

using namespace Info;

class RawLoader : public QDialog, public Ui::DialogLoadRAW {
  Q_OBJECT 

  private:
    Info::FCS_Info * info;
    QwtPlotMarker m1, m2;

    vector<int> ch1, ch2;	//wektory czasow odstepu pomiedzy kolejnymi zliczeniami dla obu kanalow (dla wczytywania traj. RAW)

  private slots:
//     void DrawFunction (const Info::Func & f, QString text, QwtPlot * plot);
    
    void CalculateIntensity ();
    void CalculateRAWCorrelation ();
    void CalcCorrRightAway ();
    void OpenRAWFile ();
    
  public:
    FCS_Info * Wynik () {return info;}
    RawLoader ();
	
//  public slots:
};


#endif //_MW_RAW_LOADER_
