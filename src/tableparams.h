#ifndef _MY_TABLE_
#define _MY_TABLE_

#include <QMessageBox>
#include <QTableWidget>
#include <QHeaderView>
#include <math.h>

namespace SM {
  enum SwitchModes {
    OFF = 0,
    ON,
    SWITCH
  };
};

class TableParams : public QTableWidget {
      Q_OBJECT

  private:

  public:
        TableParams (QWidget * parent);

  public slots:
        void lockCell (int r, SM::SwitchModes mode);
        void setValue (double v, int r, int c);

  private slots:
//        void on_cellChanged (int r,int c);
        void emit_cellValueSet (int r,int c);

  signals:
        void cellValueSet (double v, int r, int c);
        
};


#endif //_MY_TABLE_
