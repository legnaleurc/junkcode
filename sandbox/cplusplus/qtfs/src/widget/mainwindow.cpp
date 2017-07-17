#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <QtWidgets/QFileSystemModel>
#include <QtCore/QDir>

#include <QtCore/QtDebug>


namespace qtfs {

class MainWindow::Private : public QObject {
public:
    explicit Private(MainWindow * parent);

    MainWindow * _;
    Ui::MainWindow ui;
    QFileSystemModel * model;
};

}


using namespace qtfs;


MainWindow::MainWindow()
    : QMainWindow()
    , _(new Private(this))
{
}


MainWindow::Private::Private(MainWindow * parent)
    : QObject(parent)
    , _(parent)
    , ui()
    , model(new QFileSystemModel(this))
{
    this->ui.setupUi(this->_);

    this->model->setRootPath(QDir::rootPath());
    this->model->setReadOnly(false);
    this->model->setResolveSymlinks(true);

    auto filter = this->model->filter();
    filter |= QDir::Hidden | QDir::System;
    this->model->setFilter(filter);

    this->ui.left->initialize(this->model);
    this->ui.right->initialize(this->model);
}
