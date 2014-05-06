#ifndef _MY_LOG_EDIT_
#define _MY_LOG_EDIT_

#include <QLineEdit>
#include <QMessageBox>
//#include <math.h>

class LogLineEdit: public QLineEdit {
      Q_OBJECT

  public slots:
        void setValue (double v, int , int ) { setText (QString::number((int)(v+0.5))); } //+0.5 zeby lepiej zaokraglic :) , int,int - zeby sie zgadzala sygnatura z logsliderem
	void emit_cellValueSet (const QString &) {emit cellValueSet (text().toDouble(), 0,0);}

  signals:
        void cellValueSet (double v, int r, int c);

  public:
    LogLineEdit (QWidget * parent) : QLineEdit (parent) {
 	connect (this, SIGNAL (textChanged(const QString &)), this, SLOT (emit_cellValueSet (const QString &))); 
    }
};


#endif //_MY_LOG_EDIT_
