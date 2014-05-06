FCS-analyzer
============

How to compile:

- qmake
- (make clean) - first time
- make
- ./FCS_analyzer

Needs:

- libqwt 5.2.2
- QT4
- C++
- qmake
- levmar 2.4 by Manolis Lourakis and its dependencies:
	- f2c
	- libblas
	- liblapack

```
sudo apt-get install libqwt-dev liblapack libblas f2c
cd levmar-2.4; make
```

## Docs
Old version: http://docs.huihoo.com/qwt/class_qwt_plot_marker.html#eb0c9bdaeb6dfd7e86ef832fa5f8e3dd

New version:
http://qwt.sourceforge.net/class_qwt_plot_marker.html#af44231c2b63bb277706e4f515297c7b1
