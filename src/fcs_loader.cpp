#include "fcs_loader.h"

using namespace Info;
using std::cout;

QString FCS_Info :: ReadLine(QFile* fp) {
	QString line = fp->readLine();
	return line.trimmed();
}

void FCS_Info :: Print (std::ostream & out) {
	out << "Method\t\t" << method.Name.toAscii().data() << " ("<<method.Comment.toAscii().data()<<") from "<<method.Date.toAscii().data() <<"\n";
	out << "Measure date\t" << method.MeasureDate.toAscii().data() << "\n";
	out << "Number of experiments (repeats): "<< repeats<<"\n";
	for (uint c=0;c<correlation.size();c++) {
		out << "Correlation #" <<c<<":\n";
		for (int i=0;i<5;i++) {out << correlation[c][i].x << "\t" <<correlation[c][i].y << "\n";}
	}
	for (uint c=0;c<countrate.size();c++) {
		out << "Countrate #" <<c<<":\n";
		for (int i=0;i<5;i++) {out << countrate[c][i].x << "\t" <<countrate[c][i].y << "\n";}
	}
}

void FCS_Info :: ReadLasers(QFile* fp) {
}

double FCS_Info :: AvgIntensity (int i) {
       double total = 0;
       for (uint a = 0; a < countrate[i].size(); a++) total += countrate [i][a].y;
       return (total / countrate[i].size());
}

void FCS_Info :: ReadMeasureParams (QFile * fp) {
     QString line;
     int begins = 1;

     while ((line = ReadLine (fp))!=NULL) {
      QStringList words = line.split(QString(" = "),QString::SkipEmptyParts);
// 	for (int i=0;i<words.size();i++) {cout << i << ": " <<words[i].toAscii().data() << "|\n";}
      if (words[0] == "MethodName") method.Name = words[1];
      if (words[0] == "MethodComment") method.Comment = words[1];
      if (words[0] == "MethodDate") method.Date = words[1];
      if (words[0] == "MeasureDate") method.MeasureDate = words[1];

	  words = line.split (" ",QString::SkipEmptyParts); //podzielmy na poszczegolne slowa
	  if (words[0] == "BEGIN") {
		begins ++; cout << line.toAscii().data() <<"\n"; cout << begins << " ";
		if (line == "BEGIN Lasers 10000") ReadLasers (fp);
//		if (line == "BEGIN Correlations 10000") ReadCorrelationsInfo (fp); //is it needed?
	  }
 	  else if (words[0] == "END") {
		begins --;
		if (begins <= 0) {cout << "END reached, params reading finished.\n"; return;}
	  }
     }
}

//TODO? histogram zliczen, parametry fitu
void FCS_Info :: ReadCountrate(QFile* fp) {
     QString line;
     int begins = 1;

     std::cout << "Reading countrates...";////////////////////////

     while ((line = ReadLine (fp))!=NULL) {
         QStringList words = line.split(QString(" = "),QString::SkipEmptyParts);
         if (words[0] == "CountRateArrayUsage") {
		int num_channels = words[1].toInt();
		countrate[countrates_loaded].resize(num_channels);
		ReadLine (fp); //skip "CountRateArray = num_channels 2" line
		for (int i=0;i<num_channels;++i) {
			line = ReadLine (fp);
			words = line.split(QString(" "),QString::SkipEmptyParts);
			countrate[countrates_loaded][i].x = words[0].toDouble();
			countrate[countrates_loaded][i].y = words[1].toDouble();
		}
	 }
	//TODO maybe: read histogram?
	
	 if (words[0] == "BEGIN")
		{begins ++; cout << line.toAscii().data() <<"\n"; cout << begins << " ";}
	 else if (words[0] == "END")
		{begins --;if (begins <= 0) {cout << "END reached, countrate reading finished.\n"; countrates_loaded ++; return;}}
     }
}

void FCS_Info :: ReadCorrelation(QFile* fp) {
     QString line;
     int begins = 1;

     std::cout << "Reading correlations...";////////////////////////

     while ((line = ReadLine (fp))!=NULL) {
         QStringList words = line.split(QString(" = "),QString::SkipEmptyParts);
         if (words[0] == "CorrelationArrayUasge") {
		int num_channels = words[1].toInt();
		correlation[corrs_loaded].resize(num_channels);
		ReadLine (fp); //skip "CorrelationArray = num_channels 2" line
		for (int i=0;i<num_channels;++i) {
			line = ReadLine (fp);
			words = line.split(QString(" "),QString::SkipEmptyParts);
			correlation[corrs_loaded][i].x = words[0].toDouble()*10;	//NOTE w "oryginalnych" plikach .fcs jednostka jest 10 us!!
			correlation[corrs_loaded][i].y = words[1].toDouble();
		}
	 }
	
	 if (words[0] == "BEGIN")
		{begins ++; cout << line.toAscii().data() <<"\n"; cout << begins << " ";}
	 else if (words[0] == "END")
		{begins --;if (begins <= 0) {cout << "END reached, correlation reading finished.\n"; corrs_loaded ++; return;}}
     }
}

/* ------------- Glowna funkcja ladujaca plik FCS  --------------- */
bool FCS_Info :: LoadFCSFile (QString filename) {
 //check if file's ok
 QFile fp (filename);
 if (!fp.open(QIODevice::ReadOnly | QIODevice::Text)) {
	QMessageBox::warning( NULL, "Error", "Could not open file.");
	return false;
 }

 //Read header
 QString line = ReadLine (&fp);

 if (line != "Carl Zeiss ConfoCor2 - measurement data file Version 2.1") {
	QMessageBox::warning( NULL, "Error", QObject::tr("Incorrect file version: %1.").arg(line));
	return false;
 }

 while ((line = ReadLine (&fp)) != NULL) {
 	QStringList words = line.split(QString(" = "),QString::SkipEmptyParts); 
	if (words[0] == "RepeatCount") {repeats = words[1].toInt();}
	if (words[0] == "NumberPositions") {repeats = repeats * words[1].toInt(); correlation.resize(repeats); countrate.resize(repeats);} //total #correlations = repeats * NumberPositions
	if (line == "BEGIN MeasureParameters 10001")  ReadMeasureParams (&fp);
	if (line == "BEGIN Channel0 10001")  ReadCountrate (&fp);
	if (line == "BEGIN Correlation0 10001")  ReadCorrelation (&fp);
 }

 if (corrs_loaded != repeats) {std::cerr << "ERROR: only "<<corrs_loaded<<" out of "<<repeats<<" correlations loaded."; return false;}
 if (countrates_loaded != repeats) {std::cerr << "ERROR: only "<<countrates_loaded<<" out of "<<repeats<<" count rates loaded."; return false;}

 return true;
}

QString FCS_Info :: GetMeasurementInfo () {
   QString a;
   
   a += method.Name +'\n'+ method.Comment +'\n'+ method.Date +'\n'+ method.MeasureDate +'\n'+ method.User;
   return a;
}

/*
       //-----Beam Path-----//
       bp.Name = Trim(pLista->Values["BeamPathName "]);
       bp.Pinhole_Adjustment = Trim(pLista->Values["PinholeAdjustment "]);


       int CountDetectors = StrToInt(pLista->Strings[pLista->IndexOf("BEGIN Detectors 10000")+1].SubString(9,pLista->Strings[pLista->IndexOf("BEGIN Detectors 10000")+1].Length()-8));
       wiersz = pLista->IndexOf("BEGIN Detectors 10000")+3;

       for(int i=1; i<=CountDetectors;i++)
       {
         Beam1.Channel_Name +="CH"+IntToStr(i)+": "+pLista->Strings[wiersz].SubString(16,pLista->Strings[wiersz].Length()-15)+"  ";
         Beam1.Pinhole_Diameter +="CH"+IntToStr(i)+": "+pLista->Strings[wiersz+2].SubString(27,pLista->Strings[wiersz+2].Length()-26)+"  ";
         Beam1.Emission_Filter +="CH"+IntToStr(i)+": "+pLista->Strings[wiersz+7].SubString(22,pLista->Strings[wiersz+7].Length()-21)+"  ";
         Beam1.Pinhole_Position +=" CH"+IntToStr(i)+": ";

         for(int j=1;j<=3;j++)
         {
           Beam1.Pinhole_Position +=pLista->Strings[wiersz+2+j].SubString(28,pLista->Strings[wiersz+2+j].Length()-27)+"/";
         }

         wiersz =(pLista->IndexOf("BEGIN Detectors 10000")+3)+i*10;
       }


       int CountBeamsplitters = StrToInt(pLista->Strings[pLista->IndexOf("BEGIN BeamSplitters 10000")+1].SubString(9,pLista->Strings[pLista->IndexOf("BEGIN BeamSplitters 10000")+1].Length()-8));
       wiersz = pLista->IndexOf("BEGIN BeamSplitters 10000")+4;

       for(int i=1; i<=CountBeamsplitters;i++)
       {
         Beam1.Splitters +=pLista->Strings[wiersz].SubString(26,pLista->Strings[wiersz].Length()-25)+",   ";
         wiersz = (pLista->IndexOf("BEGIN BeamSplitters 10000")+4)+i*4;
       }


       int CountCollimators = StrToInt(pLista->Strings[pLista->IndexOf("BEGIN Collimators 10000")+1].SubString(9,pLista->Strings[pLista->IndexOf("BEGIN Collimators 10000")+1].Length()-8));
       wiersz = pLista->IndexOf("BEGIN Collimators 10000")+4;

       for(int i=1; i<=CountCollimators;i++)
       {
         Beam1.Collimator +=pLista->Strings[wiersz].SubString(22,pLista->Strings[wiersz].Length()-21)+",   ";
         wiersz =(pLista->IndexOf("BEGIN Collimators 10000")+4)+i*4;
       }
       //---------Lasers----------//

       int CountLasers = StrToInt(pLista->Strings[pLista->IndexOf("BEGIN Lasers 10000")+1].SubString(9,pLista->Strings[pLista->IndexOf("BEGIN Lasers 10000")+1].Length()-8));
       wiersz = pLista->IndexOf("BEGIN Lasers 10000")+3;

       float Procent;
       for(int i=1; i<=CountLasers;i++)
       {
         Laser1.Name +=IntToStr(i)+") "+pLista->Strings[wiersz].SubString(13,pLista->Strings[wiersz].Length()-12)+",  ";

         Buffer = Trim(pLista->Strings[wiersz+2].SubString(22,pLista->Strings[wiersz+2].Length()-21));
         if(DecimalSeparator == ',')
         {
           Buffer.Insert(',',Buffer.Pos('.'));
           Buffer.Delete(Buffer.Pos('.'),1);
         }
         Procent = StrToFloat(Buffer)*100;
         Buffer = FloatToStr(Procent)+"%";
         Laser1.Power +=IntToStr(i)+") "+Buffer+" of " +pLista->Strings[wiersz+1].SubString(14,pLista->Strings[wiersz+1].Length()-13)+",  ";
         wiersz = (pLista->IndexOf("BEGIN Lasers 10000")+3)+i*6;

       }

       int CountAttenuators = StrToInt(pLista->Strings[pLista->IndexOf("BEGIN Attenuators 10000")+1].SubString(9,pLista->Strings[pLista->IndexOf("BEGIN Attenuators 10000")+1].Length()-8));
       wiersz = pLista->IndexOf("BEGIN Attenuators 10000")+3;

       for (int i=1;i<=CountAttenuators;i++)
       {
         Buffer = Trim(pLista->Strings[wiersz+1].SubString(27,pLista->Strings[wiersz+1].Length()-26));

         if(DecimalSeparator == ',')
         {
           Buffer.Insert(',',Buffer.Pos('.'));
           Buffer.Delete(Buffer.Pos('.'),1);
         }
 
         Procent = StrToFloat(Buffer)*100;
         Buffer = FloatToStr(Procent)+"%";
         Laser1.Transmission += IntToStr(i)+") "+pLista->Strings[wiersz].SubString(24,pLista->Strings[wiersz].Length()-23)+" : "+ Buffer+" , ";
         wiersz = (pLista->IndexOf("BEGIN Attenuators 10000")+3)+i*5;
       }

       //----------Times-----------//
       Time1.Bleach = Trim(pLista->Values["BleachTime "]);
       Time1.Measure = Trim(pLista->Values["MeasureTime "]);


       //------RepeatCount i Kinetics------//
       Repeat_Count = StrToInt(Trim(pLista->Values["RepeatCount "]));
       Kinetics = Trim(pLista->Values["Kinetics "]);

       //-----Measure Date--------------//
       MeasureDate = Trim(pLista->Values["MeasureDate "]);
*/
