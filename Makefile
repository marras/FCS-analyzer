#############################################################################
# Makefile for building: FCS_analyzer
# Generated by qmake (2.01a) (Qt 4.8.1) on: Tue May 6 23:28:17 2014
# Project:  FCS_analyzer.pro
# Template: app
# Command: /usr/bin/qmake -o Makefile FCS_analyzer.pro
#############################################################################

####### Compiler, tools and options

CC            = gcc
CXX           = g++
DEFINES       = -DQT_WEBKIT -DQT_NO_DEBUG -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED
CFLAGS        = -m64 -pipe -O2 -Wall -W -D_REENTRANT $(DEFINES)
CXXFLAGS      = -m64 -pipe -O2 -Wall -W -D_REENTRANT $(DEFINES)
INCPATH       = -I/usr/share/qt4/mkspecs/linux-g++-64 -I. -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtGui -I/usr/include/qt4 -I. -I/usr/include/qt4 -I/usr/include/qwt -Isrc -I. -I.
LINK          = g++
LFLAGS        = -m64 -Wl,-O1
LIBS          = $(SUBLIBS)  -L./levmar-2.4 -L/usr/lib/x86_64-linux-gnu -lqwt -llevmar -llapack -lblas -lQtGui -lQtCore -lpthread 
AR            = ar cqs
RANLIB        = 
QMAKE         = /usr/bin/qmake
TAR           = tar -cf
COMPRESS      = gzip -9f
COPY          = cp -f
SED           = sed
COPY_FILE     = $(COPY)
COPY_DIR      = $(COPY) -r
STRIP         = strip
INSTALL_FILE  = install -m 644 -p
INSTALL_DIR   = $(COPY_DIR)
INSTALL_PROGRAM = install -m 755 -p
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p

####### Output directory

OBJECTS_DIR   = ./

####### Files

SOURCES       = src/analyzer.cpp \
		src/logslider.cpp \
		src/main.cpp \
		src/tableparams.cpp \
		src/fcs_loader.cpp \
		src/zoomplot.cpp \
		src/fit.cpp \
		src/raw_loader.cpp \
		src/correlatorSimple.cpp \
		src/program.cpp \
		src/correlatorPAT.cpp moc_analyzer.cpp \
		moc_logslider.cpp \
		moc_tableparams.cpp \
		moc_zoomplot.cpp \
		moc_loglineedit.cpp \
		moc_raw_loader.cpp \
		moc_program.cpp
OBJECTS       = analyzer.o \
		logslider.o \
		main.o \
		tableparams.o \
		fcs_loader.o \
		zoomplot.o \
		fit.o \
		raw_loader.o \
		correlatorSimple.o \
		program.o \
		correlatorPAT.o \
		moc_analyzer.o \
		moc_logslider.o \
		moc_tableparams.o \
		moc_zoomplot.o \
		moc_loglineedit.o \
		moc_raw_loader.o \
		moc_program.o
DIST          = /usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/common/gcc-base.conf \
		/usr/share/qt4/mkspecs/common/gcc-base-unix.conf \
		/usr/share/qt4/mkspecs/common/g++-base.conf \
		/usr/share/qt4/mkspecs/common/g++-unix.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/modules/qt_webkit_version.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		/usr/share/qt4/mkspecs/features/release.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/unix/gdb_dwarf_index.prf \
		/usr/share/qt4/mkspecs/features/warn_on.prf \
		/usr/share/qt4/mkspecs/features/qt.prf \
		/usr/share/qt4/mkspecs/features/unix/thread.prf \
		/usr/share/qt4/mkspecs/features/moc.prf \
		/usr/share/qt4/mkspecs/features/resources.prf \
		/usr/share/qt4/mkspecs/features/uic.prf \
		/usr/share/qt4/mkspecs/features/yacc.prf \
		/usr/share/qt4/mkspecs/features/lex.prf \
		/usr/share/qt4/mkspecs/features/include_source_dir.prf \
		FCS_analyzer.pro
QMAKE_TARGET  = FCS_analyzer
DESTDIR       = 
TARGET        = FCS_analyzer

first: all
####### Implicit rules

.SUFFIXES: .o .c .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o "$@" "$<"

####### Build rules

all: Makefile $(TARGET)

$(TARGET): ui_Main_analyzer.h ui_Raw_loader.h ui_Tab.h $(OBJECTS)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)

Makefile: FCS_analyzer.pro  /usr/share/qt4/mkspecs/linux-g++-64/qmake.conf /usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/common/gcc-base.conf \
		/usr/share/qt4/mkspecs/common/gcc-base-unix.conf \
		/usr/share/qt4/mkspecs/common/g++-base.conf \
		/usr/share/qt4/mkspecs/common/g++-unix.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/modules/qt_webkit_version.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		/usr/share/qt4/mkspecs/features/release.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/unix/gdb_dwarf_index.prf \
		/usr/share/qt4/mkspecs/features/warn_on.prf \
		/usr/share/qt4/mkspecs/features/qt.prf \
		/usr/share/qt4/mkspecs/features/unix/thread.prf \
		/usr/share/qt4/mkspecs/features/moc.prf \
		/usr/share/qt4/mkspecs/features/resources.prf \
		/usr/share/qt4/mkspecs/features/uic.prf \
		/usr/share/qt4/mkspecs/features/yacc.prf \
		/usr/share/qt4/mkspecs/features/lex.prf \
		/usr/share/qt4/mkspecs/features/include_source_dir.prf \
		/usr/lib/x86_64-linux-gnu/libQtGui.prl \
		/usr/lib/x86_64-linux-gnu/libQtCore.prl
	$(QMAKE) -o Makefile FCS_analyzer.pro
/usr/share/qt4/mkspecs/common/unix.conf:
/usr/share/qt4/mkspecs/common/linux.conf:
/usr/share/qt4/mkspecs/common/gcc-base.conf:
/usr/share/qt4/mkspecs/common/gcc-base-unix.conf:
/usr/share/qt4/mkspecs/common/g++-base.conf:
/usr/share/qt4/mkspecs/common/g++-unix.conf:
/usr/share/qt4/mkspecs/qconfig.pri:
/usr/share/qt4/mkspecs/modules/qt_webkit_version.pri:
/usr/share/qt4/mkspecs/features/qt_functions.prf:
/usr/share/qt4/mkspecs/features/qt_config.prf:
/usr/share/qt4/mkspecs/features/exclusive_builds.prf:
/usr/share/qt4/mkspecs/features/default_pre.prf:
/usr/share/qt4/mkspecs/features/release.prf:
/usr/share/qt4/mkspecs/features/default_post.prf:
/usr/share/qt4/mkspecs/features/unix/gdb_dwarf_index.prf:
/usr/share/qt4/mkspecs/features/warn_on.prf:
/usr/share/qt4/mkspecs/features/qt.prf:
/usr/share/qt4/mkspecs/features/unix/thread.prf:
/usr/share/qt4/mkspecs/features/moc.prf:
/usr/share/qt4/mkspecs/features/resources.prf:
/usr/share/qt4/mkspecs/features/uic.prf:
/usr/share/qt4/mkspecs/features/yacc.prf:
/usr/share/qt4/mkspecs/features/lex.prf:
/usr/share/qt4/mkspecs/features/include_source_dir.prf:
/usr/lib/x86_64-linux-gnu/libQtGui.prl:
/usr/lib/x86_64-linux-gnu/libQtCore.prl:
qmake:  FORCE
	@$(QMAKE) -o Makefile FCS_analyzer.pro

dist: 
	@$(CHK_DIR_EXISTS) .tmp/FCS_analyzer1.0.0 || $(MKDIR) .tmp/FCS_analyzer1.0.0 
	$(COPY_FILE) --parents $(SOURCES) $(DIST) .tmp/FCS_analyzer1.0.0/ && $(COPY_FILE) --parents src/analyzer.h src/logslider.h src/tableparams.h src/fcs_loader.h src/zoomplot.h src/loglineedit.h src/correlator.h src/lm.h src/raw_loader.h src/program.h .tmp/FCS_analyzer1.0.0/ && $(COPY_FILE) --parents src/analyzer.cpp src/logslider.cpp src/main.cpp src/tableparams.cpp src/fcs_loader.cpp src/zoomplot.cpp src/fit.cpp src/raw_loader.cpp src/correlatorSimple.cpp src/program.cpp src/correlatorPAT.cpp .tmp/FCS_analyzer1.0.0/ && $(COPY_FILE) --parents Main_analyzer.ui Raw_loader.ui Tab.ui .tmp/FCS_analyzer1.0.0/ && (cd `dirname .tmp/FCS_analyzer1.0.0` && $(TAR) FCS_analyzer1.0.0.tar FCS_analyzer1.0.0 && $(COMPRESS) FCS_analyzer1.0.0.tar) && $(MOVE) `dirname .tmp/FCS_analyzer1.0.0`/FCS_analyzer1.0.0.tar.gz . && $(DEL_FILE) -r .tmp/FCS_analyzer1.0.0


clean:compiler_clean 
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core


####### Sub-libraries

distclean: clean
	-$(DEL_FILE) $(TARGET) 
	-$(DEL_FILE) Makefile


check: first

mocclean: compiler_moc_header_clean compiler_moc_source_clean

mocables: compiler_moc_header_make_all compiler_moc_source_make_all

compiler_moc_header_make_all: moc_analyzer.cpp moc_logslider.cpp moc_tableparams.cpp moc_zoomplot.cpp moc_loglineedit.cpp moc_raw_loader.cpp moc_program.cpp
compiler_moc_header_clean:
	-$(DEL_FILE) moc_analyzer.cpp moc_logslider.cpp moc_tableparams.cpp moc_zoomplot.cpp moc_loglineedit.cpp moc_raw_loader.cpp moc_program.cpp
moc_analyzer.cpp: ui_Tab.h \
		src/tableparams.h \
		src/fcs_loader.h \
		src/analyzer.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) src/analyzer.h -o moc_analyzer.cpp

moc_logslider.cpp: src/logslider.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) src/logslider.h -o moc_logslider.cpp

moc_tableparams.cpp: src/tableparams.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) src/tableparams.h -o moc_tableparams.cpp

moc_zoomplot.cpp: src/zoomplot.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) src/zoomplot.h -o moc_zoomplot.cpp

moc_loglineedit.cpp: src/loglineedit.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) src/loglineedit.h -o moc_loglineedit.cpp

moc_raw_loader.cpp: ui_Raw_loader.h \
		src/analyzer.h \
		ui_Tab.h \
		src/tableparams.h \
		src/fcs_loader.h \
		src/raw_loader.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) src/raw_loader.h -o moc_raw_loader.cpp

moc_program.cpp: ui_Main_analyzer.h \
		src/analyzer.h \
		ui_Tab.h \
		src/tableparams.h \
		src/fcs_loader.h \
		src/program.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) src/program.h -o moc_program.cpp

compiler_rcc_make_all:
compiler_rcc_clean:
compiler_image_collection_make_all: qmake_image_collection.cpp
compiler_image_collection_clean:
	-$(DEL_FILE) qmake_image_collection.cpp
compiler_moc_source_make_all:
compiler_moc_source_clean:
compiler_uic_make_all: ui_Main_analyzer.h ui_Raw_loader.h ui_Tab.h
compiler_uic_clean:
	-$(DEL_FILE) ui_Main_analyzer.h ui_Raw_loader.h ui_Tab.h
ui_Main_analyzer.h: Main_analyzer.ui
	/usr/bin/uic-qt4 Main_analyzer.ui -o ui_Main_analyzer.h

ui_Raw_loader.h: Raw_loader.ui
	/usr/bin/uic-qt4 Raw_loader.ui -o ui_Raw_loader.h

ui_Tab.h: Tab.ui
	/usr/bin/uic-qt4 Tab.ui -o ui_Tab.h

compiler_yacc_decl_make_all:
compiler_yacc_decl_clean:
compiler_yacc_impl_make_all:
compiler_yacc_impl_clean:
compiler_lex_make_all:
compiler_lex_clean:
compiler_clean: compiler_moc_header_clean compiler_uic_clean 

####### Compile

analyzer.o: src/analyzer.cpp src/analyzer.h \
		ui_Tab.h \
		src/tableparams.h \
		src/fcs_loader.h \
		src/program.h \
		ui_Main_analyzer.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o analyzer.o src/analyzer.cpp

logslider.o: src/logslider.cpp src/logslider.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o logslider.o src/logslider.cpp

main.o: src/main.cpp src/program.h \
		ui_Main_analyzer.h \
		src/analyzer.h \
		ui_Tab.h \
		src/tableparams.h \
		src/fcs_loader.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o main.o src/main.cpp

tableparams.o: src/tableparams.cpp src/tableparams.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o tableparams.o src/tableparams.cpp

fcs_loader.o: src/fcs_loader.cpp src/fcs_loader.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o fcs_loader.o src/fcs_loader.cpp

zoomplot.o: src/zoomplot.cpp src/zoomplot.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o zoomplot.o src/zoomplot.cpp

fit.o: src/fit.cpp src/analyzer.h \
		ui_Tab.h \
		src/tableparams.h \
		src/fcs_loader.h \
		src/lm.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o fit.o src/fit.cpp

raw_loader.o: src/raw_loader.cpp src/raw_loader.h \
		ui_Raw_loader.h \
		src/analyzer.h \
		ui_Tab.h \
		src/tableparams.h \
		src/fcs_loader.h \
		src/correlator.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o raw_loader.o src/raw_loader.cpp

correlatorSimple.o: src/correlatorSimple.cpp src/correlator.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o correlatorSimple.o src/correlatorSimple.cpp

program.o: src/program.cpp src/program.h \
		ui_Main_analyzer.h \
		src/analyzer.h \
		ui_Tab.h \
		src/tableparams.h \
		src/fcs_loader.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o program.o src/program.cpp

correlatorPAT.o: src/correlatorPAT.cpp src/correlator.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o correlatorPAT.o src/correlatorPAT.cpp

moc_analyzer.o: moc_analyzer.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_analyzer.o moc_analyzer.cpp

moc_logslider.o: moc_logslider.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_logslider.o moc_logslider.cpp

moc_tableparams.o: moc_tableparams.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_tableparams.o moc_tableparams.cpp

moc_zoomplot.o: moc_zoomplot.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_zoomplot.o moc_zoomplot.cpp

moc_loglineedit.o: moc_loglineedit.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_loglineedit.o moc_loglineedit.cpp

moc_raw_loader.o: moc_raw_loader.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_raw_loader.o moc_raw_loader.cpp

moc_program.o: moc_program.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_program.o moc_program.cpp

####### Install

install:   FORCE

uninstall:   FORCE

FORCE:

