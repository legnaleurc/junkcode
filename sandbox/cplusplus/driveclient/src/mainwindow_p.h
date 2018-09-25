#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "drivemodel.h"

#include <QtWidgets/QMenu>
#include <QtWidgets/QAction>


class MainWindowPrivate : public QObject {
    Q_OBJECT
public:
    explicit MainWindowPrivate(MainWindow * q);

    void setupTreeView(QTreeView * view);

public slots:
    void showContextMenu(const QPoint & pos);

public:
    MainWindow * q;
    Ui_MainWindow ui;
    DriveModel * model;
    QMenu * contextMenu;
    QAction * copyUrl;
};

#endif
