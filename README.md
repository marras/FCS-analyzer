FCS-analyzer
============

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

## Docs
Old version: http://docs.huihoo.com/qwt/class_qwt_plot_marker.html#eb0c9bdaeb6dfd7e86ef832fa5f8e3dd

New version:
http://qwt.sourceforge.net/class_qwt_plot_marker.html#af44231c2b63bb277706e4f515297c7b1
