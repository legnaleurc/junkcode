#-------------------------------------------------
#
# Project created by QtCreator 2012-02-21T20:29:03
#
#-------------------------------------------------

QT       += core gui
CONFIG	+= debug_and_release

TARGET = qsnapshot
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    snapshottimer.cpp \
    abstractgrabber.cpp \
    regiongrabber.cpp \
	windowgrabber.cpp

win32:SOURCES += uiinspector_win.cpp
unix:SOURCES += uiinspector_x11.cpp

unix:QMAKE_CXXFLAGS += -std=c++0x

HEADERS  += mainwindow.hpp \
    mainwindowprivate.hpp \
    snapshottimer.hpp \
    snapshottimerprivate.hpp \
    abstractgrabber.hpp \
    regiongrabber.hpp \
    regiongrabberprivate.hpp \
    windowgrabber.hpp \
    uiinspector.hpp \
    windowgrabberprivate.hpp

FORMS    += mainwindow.ui
