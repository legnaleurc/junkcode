#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <QtWidgets/QFileSystemModel>
#include <QtCore/QDir>


namespace qtfs {

class MainWindow::Private : public QObject {
public:
    Private(MainWindow * parent);

    void setupModel(QFileSystemModel * model, QTreeView * view);

    MainWindow * _;
    Ui::MainWindow ui;
    QFileSystemModel * leftModel;
    QFileSystemModel * rightModel;
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
    , leftModel(new QFileSystemModel(this))
    , rightModel(new QFileSystemModel(this))
{
    this->ui.setupUi(this->_);

    this->setupModel(this->leftModel, this->ui.leftView);
    this->setupModel(this->rightModel, this->ui.rightView);
}


void MainWindow::Private::setupModel(QFileSystemModel * model, QTreeView * view) {
    model->setRootPath(QDir::homePath());
    view->setModel(model);
}
