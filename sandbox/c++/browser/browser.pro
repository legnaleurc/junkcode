#-------------------------------------------------
#
# Project created by QtCreator 2014-07-10T17:21:18
#
#-------------------------------------------------

QT       += core gui webkit testlib sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets webkitwidgets

TARGET = browser
TEMPLATE = app

QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7
QMAKE_CXXFLAGS += -std=c++11 -stdlib=libc++ -F ApplicationServices
QMAKE_LFLAGS += -lc++

CONFIG += link_pkgconfig
PKGCONFIG += opencv


SOURCES += main.cpp\
		mainwindow.cpp \
    networkaccessmanagerproxy.cpp \
    webpage.cpp \
    robot.cpp \
    macrobot.cpp \
    database.cpp \
    controller.cpp \
    eventqueue.cpp \
    mouse.cpp

HEADERS  += mainwindow.h \
    networkaccessmanagerproxy.h \
    webpage.h \
    robot.h \
    macrobot.h \
    database.h \
    controller.h \
    eventqueue.h \
    mouse.h

FORMS    += mainwindow.ui

OTHER_FILES +=

RESOURCES += \
    resource.qrc
