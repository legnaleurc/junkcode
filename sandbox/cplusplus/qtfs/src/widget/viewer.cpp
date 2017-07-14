#include "viewer.hpp"
#include "ui_viewer.h"

#include <QtWidgets/QFileSystemModel>
#include <QtCore/QDir>
#include <QtWidgets/QCompleter>
#include <QtWidgets/QMenu>

#include <QtCore/QtDebug>


namespace qtfs {

class Viewer::Private : public QObject {
    Q_OBJECT

public:
    explicit Private(Viewer * parent);
    void setPath(const QString & path);

public slots:
    void commitPath();
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


Viewer::Private::Private(Viewer * parent)
    : QObject(parent)
    , _(parent)
    , ui()
    , menu(new QMenu(parent))
    , model(new QFileSystemModel(this))
    , currentContextMenuPath()
{
    this->ui.setupUi(this->_);

    this->model->setRootPath(QDir::rootPath());
    this->model->setReadOnly(false);

    this->ui.view->setModel(this->model);
    this->ui.view->sortByColumn(0, Qt::AscendingOrder);

    auto completer = new QCompleter(this->model, this);
    this->ui.path->setCompleter(completer);

    this->connect(this->ui.path, SIGNAL(returnPressed()), SLOT(commitPath()));

    this->connect(this->ui.up, SIGNAL(pressed()), SLOT(goUp()));

    this->connect(this->ui.view, SIGNAL(customContextMenuRequested(const QPoint &)), SLOT(showContextMenu(const QPoint &)));

    auto renameAction = this->menu->addAction(QObject::tr("Rename"));
    renameAction->setData(QVariant::fromValue(ContextMenuAction::RENAME));
    auto deleteAction = this->menu->addAction(QObject::tr("Delete"));
    deleteAction->setData(QVariant::fromValue(ContextMenuAction::DELETE));
    this->connect(this->menu, SIGNAL(triggered(QAction *)), SLOT(contextMenuTriggered(QAction *)));

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
