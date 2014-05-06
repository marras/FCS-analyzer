#ifndef _MW_ANALYZER_H_
#define _MW_ANALYZER_H_

#include "ui_Tab.h"
#include <QMessageBox>
#include <QFileDialog>
//#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_engine.h>
#include <qwt_plot_marker.h>
#include "tableparams.h"
#include "fcs_loader.h"

#define NUM_PARAMS 11

double G_val (double tau,  const double par [NUM_PARAMS]);

class Analyzer : public QWidget, public Ui::Tab {
  Q_OBJECT 

  private:
    Info::FCS_Info * info;
    QwtPlotMarker m1, m2;

  private slots:
    bool CalculateAverageCorrelation ();
    void DrawCorrelationFromParams ();
    void setFitLimits(int button, double x, double y);
    void setFitModel (int model);
    void setBleachModel (int s);
    void setTripletModel (int s);
    void SaveASCII ();
    void SaveAvg ();
    void SaveParams ();
    void ShowInfo ();
    
  public:
    Analyzer (QWidget * parent);
    ~Analyzer ();

    QString filename;
    
    void OpenFCSFile (QString fname);
    void OpenASCII ();
    void OpenRAWFile ();
    void OpenSimulated (QString fname); 
    
  public slots:
    bool Fit ();
};


void DrawFunction (const Info::Func & f, QString text, QwtPlot * plot); //now global

class IndexedPushButton : public QPushButton {
  Q_OBJECT

  private:     
      int i;
      
  private slots:
      void emit_i ();    
      
  public: 
      IndexedPushButton (QString name, int _i) : QPushButton (name) {i = _i; connect (this,SIGNAL(clicked()),this,SLOT(emit_i()));}
      
  signals:
      void clicked (int, SM::SwitchModes);
};

#endif //_MW_ANALYZER_H_
