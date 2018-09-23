#include "mainwindow_p.h"


MainWindow::MainWindow(QWidget * parent)
    : QMainWindow(parent)
    , d(new MainWindowPrivate(this))
{
    d->ui.setupUi(this);
    d->model->setRootPath("localhost", 8000);
    d->ui.leftView->setModel(d->model);
    d->ui.rightView->setModel(d->model);
}


MainWindowPrivate::MainWindowPrivate(MainWindow * q)
    : QObject(q)
    , q(q)
    , ui()
    , model(new DriveModel(this))
{
}
