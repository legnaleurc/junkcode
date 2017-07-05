#include "viewer.hpp"
#include "ui_viewer.h"

#include <QtWidgets/QFileSystemModel>
#include <QtCore/QDir>
#include <QtWidgets/QCompleter>


namespace qtfs {

class Viewer::Private : public QObject {
public:
    explicit Private(Viewer * parent);

    Viewer * _;
    Ui::Viewer ui;
    QFileSystemModel * model;
};

}


using namespace qtfs;


Viewer::Viewer(QWidget * parent)
    : QWidget(parent)
    , _(new Private(this))
{
}


Viewer::Private::Private(Viewer * parent)
    : QObject(parent)
    , _(parent)
    , ui()
    , model(new QFileSystemModel(this))
{
    this->ui.setupUi(this->_);

    this->model->setRootPath(QDir::rootPath());
    this->ui.view->setModel(this->model);
    this->ui.view->setRootIndex(this->model->index(QDir::homePath()));

    QCompleter * completer = new QCompleter(this->model, this);
    this->ui.path->setCompleter(completer);
}
