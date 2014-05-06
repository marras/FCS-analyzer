#include "zoomplot.h"

#include <QMessageBox>
#include <qwt_scale_widget.h>
#include <qwt_scale_div.h>
#include <QSize>
#include <QMoveEvent>

//overriden virtual protected function from QWidget :)
void ZoomPlot :: mousePressEvent ( QMouseEvent * event ) {
  if (event->button() == Qt::LeftButton) { //LMB - zooming
     zoom_start_x = event->x();// - axisWidget(yLeft)->sizeHint().width();
     zoom_start_y = event->y(); // - axis->sizeHint().width(); //na gorze nie ma skali, wiec nie trzeba :)

     kwadrat = new KwadratWidget(this);
     kwadrat->x1 = zoom_start_x; kwadrat->y1 = zoom_start_y;
     kwadrat->x2 = zoom_start_x; kwadrat->y2 = zoom_start_y;
//      std::cout << "Setting up a new KwadratWidget at "<<event->x()<<","<<event->y();
     drawing = true;
  }
/*  else if (event->button() == Qt::MidButton) {

  } //ustaw lewy kraniec
  else if (event->button() == Qt::RightButton) {
     emit clicked(Qt::RightButton, invTransform (xBottom, event->x()-axisWidget(yLeft)->sizeHint().width()), invTransform (yLeft, event->y()));
  } //ustaw prawy kraniec*/
}

void ZoomPlot :: mouseMoveEvent (QMouseEvent * event) {
  if (drawing) {
    kwadrat->x2 = event->x(); kwadrat->y2 = event->y();
    kwadrat->repaint();
  }
}

void ZoomPlot :: wheelEvent (QWheelEvent * event) {
// std::cout << "Wheel: "<<event->delta()<<"\n";  //delta == -120 - w dol
 if (event->delta() < 0) { ZoomOut(); }
}

void ZoomPlot :: mouseReleaseEvent ( QMouseEvent * event ) {
  int mx = event->x() - axisWidget(yLeft)->sizeHint().width();	//current mouse coordinates
  int my = event->y();						//in reference to plot origin (w/o scale)

  if (event->button() == Qt::LeftButton) { //LMB - zooming
     drawing = false;
     delete kwadrat;

     zoom_start_x -= axisWidget(yLeft)->sizeHint().width();

     if (mx == zoom_start_x || my == zoom_start_y) {std::cout << "Cannot zoom!\n"; return;}

     double to_x = invTransform (xBottom, mx), to_y = invTransform (yLeft, my);
     double from_x = invTransform (xBottom, zoom_start_x), from_y = invTransform (yLeft, zoom_start_y);

     ZoomIn (from_x,from_y,to_x,to_y);
  }
  else if (event->button() == Qt::MidButton) { emit clicked(Qt::MidButton, invTransform (xBottom, event->x()-axisWidget(yLeft)->sizeHint().width()), invTransform (yLeft, event->y()));} //ustaw lewy kraniec?
  else if (event->button() == Qt::RightButton) { emit clicked(Qt::RightButton, invTransform (xBottom, event->x()-axisWidget(yLeft)->sizeHint().width()), invTransform (yLeft, event->y()));} //ustaw prawy kraniec?
}

ZoomPlot :: ZoomPlot (QWidget * parent) : QwtPlot (parent) {drawing = false;}
ZoomPlot :: ~ZoomPlot () {}

void swap (double & a, double & b) {
  double c = a; a = b; b = c;
}

void ZoomPlot :: ZoomIn (double from_x, double from_y, double to_x, double to_y) {
  std::cout << "Zooming from "<<from_x<<","<<from_y<<" to "<<to_x<<","<<to_y<<std::endl;

  if (from_x > to_x) swap (from_x, to_x);
  if (from_y > to_y) swap (from_y, to_y);

  setAxisScale (xBottom, from_x, to_x);
  setAxisScale (yLeft, from_y, to_y);
  replot();
}

// void ZoomPlot :: SaveScaleParams (double x1, double x2, double y1, double y2) {
// //xmin = x1; xmax = x2; ymin = y1; ymax = y2;
// }

void ZoomPlot :: ZoomOut () {
/*	setAxisScale (xBottom, xmin, xmax);
	setAxisScale (yLeft, ymin, ymax);*/
	setAxisAutoScale (xBottom);setAxisAutoScale (yLeft);
	replot();
}
