#ifndef _MY_LOG_SLIDER_
#define _MY_LOG_SLIDER_

#include <QMessageBox>
#include <QSlider>
#include <math.h>

#ifndef ASSERT
 #ifdef assert 
  #define ASSERT assert
 #else
  inline void ASSERT (bool warunek) {if (!warunek) {QMessageBox::critical( NULL, "Error:", "Assertion failed!"); abort();}}
 #endif //assert
#endif //ASSERT


class LogSlider : public QSlider {
      Q_OBJECT

  private:
	double k;
	double A;
    
    int r,c; //row & column of the table in which the slider value is shown (optional)

  public:
    LogSlider (QWidget * parent);
    void SetExpParams (double min, double max, int r, int c); //given the return-value limits, set Base, A and table positions.

  private slots:
	void calcExpValue ();
    void updateSlider (double v,int r,int c);  //move the slider to the correct position when a value in the table is manually changed
    
  signals:
	void sendExp(double,int,int);

};


#endif //_MY_LOG_SLIDER_
