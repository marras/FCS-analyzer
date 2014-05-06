#ifndef _FCS_LOADER_H_ 
#define _FCS_LOADER_H_ 

#include <QString>
#include <QFile>
#include <QMessageBox>

#include <vector>
#include <iostream>

using std::vector;

namespace Info {
	struct Method   //przechowuje dane do info
	{
	 public:
		QString Name;
		QString Comment;
		QString Date;
		QString MeasureDate;
		QString User;
	};
	
	struct  Beam_Path
	{
	 public:
		QString Name;
		QString Channel_Name;
		QString Splitters;
		QString Emission_Filter;
		QString Pinhole_Adjustment;
		QString Pinhole_Position;
		QString Pinhole_Diameter;
		QString Collimator;
	};
	
	struct Laser
	{
	 public:
		QString Name;
		QString Transmission;
		QString Power;
	};
	
	struct Times
	{
	 public:
		QString Bleach;
		QString Measure;
	};
	
	struct XYData {
		double x;
		double y;
	};
	
	typedef vector<XYData> Func;

	class FCS_Info {
	 private:
		Method method;
		Beam_Path bp;
		Laser laser;
		Times times;

		int corrs_loaded;
		int countrates_loaded;

	   QString ReadLine(QFile* fp);
	   void ReadMeasureParams (QFile * fp);
	   void ReadLasers(QFile* fp);
	   void ReadCorrelation(QFile* fp);
	   void ReadCountrate(QFile* fp);

	 public:
	   FCS_Info () : repeats(0) {countrates_loaded = corrs_loaded = 0;}
	   FCS_Info (QString filename) {LoadFCSFile (filename);}
	   ~FCS_Info () {}
	   bool LoadFCSFile (QString filename);

	   bool LoadRAWFile (QString filename);

	   QString GetMeasurementInfo ();
	   
	   void Print (std::ostream & out);
	   double AvgIntensity (int i);

	   vector <Func> correlation;
	   vector <Func> countrate;
	   uint repeats;	//liczba powtorzen eksperymentu
	   inline int num_repeats () {if (repeats!=correlation.size()) QMessageBox::warning( NULL, "Error", "Info :: Repeats != correlation.size()!!!."); return correlation.size();}
	};
}

#endif //_FCS_LOADER_H_
