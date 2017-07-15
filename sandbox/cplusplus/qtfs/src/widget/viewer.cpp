#include "viewer.hpp"
#include "ui_viewer.h"

#include <QtWidgets/QFileSystemModel>
#include <QtCore/QDir>
#include <QtWidgets/QCompleter>
#include <QtWidgets/QMenu>

#include <QtCore/QtDebug>

#include <cassert>


namespace qtfs {

class Viewer::Private : public QObject {
    Q_OBJECT

public:
    explicit Private(Viewer * parent);
    void initialize(QFileSystemModel * model);
    void setPath(const QString & path);

public slots:
    void changePathByIndex(const QModelIndex & index);
    void changePathByInput();
    void goUp();
    void showContextMenu(const QPoint & position);
    void contextMenuTriggered(QAction * action);

public:
    Viewer * _;
    Ui::Viewer ui;
    QMenu * menu;
    QFileSystemModel * model;
    QString currentContextMenuPath;
};


enum class ContextMenuAction : uint8_t {
    RENAME,
    DELETE,
    UNKNOWN,
};

}


Q_DECLARE_METATYPE(qtfs::ContextMenuAction);


using namespace qtfs;


Viewer::Viewer(QWidget * parent)
    : QWidget(parent)
    , _(new Private(this))
{
}


void Viewer::initialize(QFileSystemModel * model) {
    _->initialize(model);
}


Viewer::Private::Private(Viewer * parent)
    : QObject(parent)
    , _(parent)
    , ui()
    , menu(new QMenu(parent))
    , model(nullptr)
    , currentContextMenuPath()
{
    this->ui.setupUi(this->_);

    this->connect(this->ui.path, SIGNAL(returnPressed()), SLOT(changePathByInput()));

    this->connect(this->ui.up, SIGNAL(pressed()), SLOT(goUp()));

    this->connect(this->ui.view, SIGNAL(doubleClicked(const QModelIndex &)), SLOT(changePathByIndex(const QModelIndex &)));
    this->connect(this->ui.view, SIGNAL(customContextMenuRequested(const QPoint &)), SLOT(showContextMenu(const QPoint &)));

    auto renameAction = this->menu->addAction(QObject::tr("Rename"));
    renameAction->setData(QVariant::fromValue(ContextMenuAction::RENAME));
    auto deleteAction = this->menu->addAction(QObject::tr("Delete"));
    deleteAction->setData(QVariant::fromValue(ContextMenuAction::DELETE));
    this->connect(this->menu, SIGNAL(triggered(QAction *)), SLOT(contextMenuTriggered(QAction *)));
}


void Viewer::Private::initialize(QFileSystemModel * model) {
    assert(model || !"initialize with a null model");
    assert(!this->model || !"should not have any model");

    this->model = model;

    this->model->setRootPath(QDir::rootPath());
    this->model->setReadOnly(false);

    this->ui.view->setModel(this->model);
    this->ui.view->sortByColumn(0, Qt::AscendingOrder);

    auto completer = new QCompleter(this->model, this);
    this->ui.path->setCompleter(completer);

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


void Viewer::Private::changePathByIndex(const QModelIndex & index) {
    auto path = this->model->filePath(index);
    this->setPath(path);
}


void Viewer::Private::changePathByInput() {
    auto path = this->ui.path->text();
    this->setPath(path);
}


void Viewer::Private::goUp() {
    auto path = this->ui.path->text();
    QDir folder(path);
    if (folder.cdUp()) {
        this->setPath(folder.path());
    }
}


void Viewer::Private::showContextMenu(const QPoint & position) {
    auto index = this->ui.view->indexAt(position);
    auto path = this->model->filePath(index);
    this->currentContextMenuPath = path;

    auto globalPosition = this->ui.view->viewport()->mapToGlobal(position);
    this->menu->popup(globalPosition);
}


void Viewer::Private::contextMenuTriggered(QAction * action) {
    if (this->currentContextMenuPath.isEmpty()) {
        return;
    }
    auto cma = action->data().isValid() ? action->data().value<ContextMenuAction>() : ContextMenuAction::UNKNOWN;
    switch (cma) {
        case ContextMenuAction::RENAME:
            qDebug() << "mv" << this->currentContextMenuPath;
            break;
        case ContextMenuAction::DELETE:
            qDebug() << "rm" << this->currentContextMenuPath;
            break;
        default:
            break;
    }
}


#include "viewer.moc"
