FCS-analyzer
============

GUI for fitting fluorescence correlation functions obtained from Carl Zeiss
Confocor 2 microscope.

###Install required libraries:

- libqwt 5.2.2
- QT4
- C++
- qmake
- levmar 2.4 by Manolis Lourakis and its dependencies:
	- f2c
	- libblas
	- liblapack

Tested with Ubuntu 14.04.
```
sudo apt-get install libqwt-dev liblapack-dev libblas-dev f2c
cd levmar-2.4; make
```

###How to compile:

- qmake
- (make clean) - first time
- make
- ./FCS_analyzer

## Docs for the plotting library (qwt)
http://qwt.sourceforge.net/class_qwt_plot_marker.html#af44231c2b63bb277706e4f515297c7b1
