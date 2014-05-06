#ifndef _ZOOMPLOT_H_
#define _ZOOMPLOT_H_

#include <qwt_plot.h>
#include <QMouseEvent>
#include <QPainter>
#include <QWidget>
#include <iostream>

class KwadratWidget : public QWidget {
 protected:
/*   void moveEvent (QMoveEvent * me) {
	std::cout << "Widget moved to "<<pos().x()<<","<<pos().y()<<".\n";
   }*/
   virtual void paintEvent (QPaintEvent *) {
	QPainter painter (this);
	painter.setPen (QColor(255,0,0));
	painter.setBrush (QColor(255,20,0,120));
	painter.drawRect (x1,y1,x2-x1,y2-y1);
// 	std::cout << "Drawing (x,y,w,h): "<< x1<<" "<< y1<<" "<<x2<<" "<< y2;
   }
 public:
  int x1,y1,x2,y2;
 
  KwadratWidget (QWidget * parent): QWidget (parent) {
	x1=y1=x2=y2=0;
	setFixedSize(800,800);show();
  }
};

class ZoomPlot : public QwtPlot {
  Q_OBJECT

  protected:
    //variables
    int zoom_start_x, zoom_start_y;
    KwadratWidget * kwadrat;
    bool drawing;

    double xmin,ymin, xmax,ymax;

    //my functions
    void ZoomIn (double from_x, double from_y, double to_x, double to_y);
    void ZoomOut ();

    //overriden from QWidget
    void mousePressEvent ( QMouseEvent * event );
    void mouseReleaseEvent ( QMouseEvent * event );
    void mouseMoveEvent ( QMouseEvent * event );
    void wheelEvent (QWheelEvent * event);
//     void paintEvent ( QPaintEvent * event);

  public:
    ZoomPlot (QWidget * parent);
    ~ZoomPlot ();

  signals:
  void clicked (int button, double x, double y);
};



#endif //_ZOOMPLOT_H_
