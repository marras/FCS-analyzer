#include "tableparams.h"
#include <iostream>
TableParams :: TableParams (QWidget * parent) : QTableWidget (parent)
{

 horizontalHeader()->setResizeMode (QHeaderView::Fixed);//ResizeToContents);
 horizontalHeader()->setDefaultSectionSize (95);
 std::cout << "size:"<< horizontalHeader()->sectionSize(0);

//	QMessageBox::information( NULL, "Tytul",tr("Section 0: %1").arg(x));
 connect (this, SIGNAL (cellChanged (int, int)), this, SLOT (emit_cellValueSet (int, int)));
}

void TableParams :: emit_cellValueSet (int r, int c) {
     emit cellValueSet (item(r,c)->text().toDouble(), r,c);
}

void TableParams :: setValue (double v, int row, int column) {
     item(row,column)->setText (QString::number(v,'g',5));
}

void TableParams :: lockCell (int r, SM::SwitchModes mode) {
 if (mode == SM::SWITCH) {
   if (item(r,0)->background() == Qt::gray) item(r,0)->setBackground(Qt::white);
   else item(r,0)->setBackground(Qt::gray); 
 }
 else if (mode == SM::OFF) item(r,0)->setBackground(Qt::gray);
 else if (mode == SM::ON) item(r,0)->setBackground(Qt::white);
  
  //TODO: spr. ENABLED? HACK Nie trzeba - przeciez mozemy chciec miec mozliwosc zmieniania 
  //wszystkich parametrow, one maja byc "stale" tylko na czas fitowania!
}
