#include "logslider.h"

LogSlider :: LogSlider (QWidget * parent) : QSlider (parent)
{
//	QMessageBox::information( NULL, "Tytul","LogSlider created :)"); 
	k = 10.0;
	A = 1.0;

	connect (this, SIGNAL(valueChanged(int)), this, SLOT(calcExpValue()));
}

void LogSlider :: calcExpValue () {
	  double a = A * exp(k*value());
      if (isSliderDown()) emit sendExp(a,r,c);
}
//zamiast valueChanged(int)

void LogSlider :: SetExpParams (double min, double max, int row, int col) {
     r=row; c=col;
     ASSERT (minimum() == 0);
     A = min; //assuming minimum = 0 ticks
     k = log(max/A) / maximum();
//  QMessageBox::information( NULL, "LogParams:", tr("%1, %2").arg(A).arg(k));     /////////
}

void LogSlider :: updateSlider (double v, int row, int col) {
     if (r!=row || c!=col) return;
     int pos = (int) (log(v/A)/k); //no need to check boundaries, setSliderPosition() will do it
     setSliderPosition (pos); //this way, with tracking off, valueChanged() signal is not emitted :) HACK?
}
