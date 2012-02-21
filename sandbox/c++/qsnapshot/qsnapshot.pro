#-------------------------------------------------
#
# Project created by QtCreator 2012-02-21T20:29:03
#
#-------------------------------------------------

QT       += core gui

TARGET = qsnapshot
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    snapshottimer.cpp \
    abstractgrabber.cpp \
    regiongrabber.cpp

HEADERS  += mainwindow.hpp \
    mainwindowprivate.hpp \
    snapshottimer.hpp \
    snapshottimerprivate.hpp \
    abstractgrabber.hpp \
    regiongrabber.hpp \
    regiongrabberprivate.hpp

FORMS    += mainwindow.ui
