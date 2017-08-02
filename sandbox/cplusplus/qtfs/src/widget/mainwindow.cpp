#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include "model/filesystemmodel.hpp"

#include <QtCore/QDir>
#include <QtWidgets/QProgressBar>

#include <QtCore/QtDebug>


namespace qtfs {

class MainWindow::Private : public QObject {
    Q_OBJECT

public:
    explicit Private(MainWindow * parent);

public slots:
    void onProgressUpdated(const FileProgressData & data);

public:
    MainWindow * _;
    Ui::MainWindow ui;
    FileSystemModel * model;
    QProgressBar * progress;
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
    , model(new FileSystemModel(this))
    , progress(new QProgressBar)
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

    this->ui.statusbar->addPermanentWidget(this->progress);
    progress->hide();
}


void MainWindow::Private::onProgressUpdated(const qtfs::FileProgressData & data) {
    if (data.finished()) {
        this->progress->hide();
        return;
    }
    this->progress->show();
    this->progress->setMaximum(data.total());
    this->progress->setValue(data.current());
}


#include "mainwindow.moc"
