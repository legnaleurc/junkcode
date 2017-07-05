#include "mainwindow.hpp"
#include "ui_mainwindow.h"


namespace qtfs {

class MainWindow::Private : public QObject {
public:
    Private(MainWindow * parent);

    MainWindow * _;
    Ui::MainWindow ui;
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
{
    this->ui.setupUi(this->_);
}
