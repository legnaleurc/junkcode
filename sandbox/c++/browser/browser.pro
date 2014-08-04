#-------------------------------------------------
#
# Project created by QtCreator 2014-07-10T17:21:18
#
#-------------------------------------------------

QT       += core gui webkit testlib sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets webkitwidgets

TARGET = browser
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11

CONFIG += link_pkgconfig
PKGCONFIG += opencv


SOURCES += main.cpp\
		mainwindow.cpp \
    networkaccessmanagerproxy.cpp \
    webpage.cpp \
    robot.cpp \
    database.cpp \
    controller.cpp \
    task.cpp

HEADERS  += mainwindow.h \
    networkaccessmanagerproxy.h \
    webpage.h \
    robot.h \
    database.h \
    controller.h \
    task.h \
    task_p.h

FORMS    += mainwindow.ui

OTHER_FILES +=

RESOURCES += \
    resource.qrc


macx {
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7
    QMAKE_CXXFLAGS += -stdlib=libc++

    INCLUDEPATH += /usr/local/Cellar/boost/1.55.0_2/include
    LIBS += -lc++ -framework ApplicationServices -L/usr/local/lib -lboost_coroutine-mt -lboost_system-mt

    SOURCES += macrobot.cpp
    HEADERS += macrobot.h
}
