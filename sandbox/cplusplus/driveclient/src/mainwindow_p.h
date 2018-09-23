#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "drivemodel.h"

class MainWindowPrivate : public QObject {
public:
    explicit MainWindowPrivate(MainWindow * q);

    MainWindow * q;
    Ui_MainWindow ui;
    DriveModel * model;
};

#endif
