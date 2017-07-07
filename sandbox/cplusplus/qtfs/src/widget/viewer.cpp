#include "viewer.hpp"
#include "ui_viewer.h"

#include <QtWidgets/QFileSystemModel>
#include <QtCore/QDir>
#include <QtWidgets/QCompleter>


namespace qtfs {

class Viewer::Private : public QObject {
    Q_OBJECT

public:
    explicit Private(Viewer * parent);
    void setPath(const QString & path);

public slots:
    void commitPath();

public:
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
    this->model->setReadOnly(false);

    this->ui.view->setModel(this->model);
    this->ui.view->sortByColumn(0, Qt::AscendingOrder);

    auto completer = new QCompleter(this->model, this);
    this->ui.path->setCompleter(completer);

    this->connect(this->ui.path, SIGNAL(returnPressed()), SLOT(commitPath()));

    this->setPath(QDir::homePath());
}


void Viewer::Private::setPath(const QString & path) {
    auto index = this->model->index(path);
    if (!index.isValid()) {
        return;
    }
    this->ui.path->setText(path);
    this->ui.view->setRootIndex(index);
}


void Viewer::Private::commitPath() {
    auto path = this->ui.path->text();
    this->setPath(path);
}


#include "viewer.moc"
