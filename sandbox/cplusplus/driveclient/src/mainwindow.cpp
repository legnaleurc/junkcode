#include "mainwindow_p.h"


MainWindow::MainWindow(QWidget * parent)
    : QMainWindow(parent)
    , d(new MainWindowPrivate(this))
{
    d->ui.setupUi(this);
    d->model->setRootPath("localhost", 8080);
    d->setupTreeView(d->ui.leftView);
    d->setupTreeView(d->ui.rightView);
}


MainWindowPrivate::MainWindowPrivate(MainWindow * q)
    : QObject(q)
    , q(q)
    , ui()
    , model(new DriveModel(this))
    , contextMenu(new QMenu(q))
    , copyUrl(new QAction("copy url", q))
{
    this->contextMenu->addAction(this->copyUrl);
}


void MainWindowPrivate::setupTreeView(QTreeView * view) {
    view->setModel(this->model);
    QObject::connect(view, &QTreeView::customContextMenuRequested, this, &MainWindowPrivate::showContextMenu);
}


void MainWindowPrivate::showContextMenu(const QPoint & pos) {
    auto view = static_cast<QTreeView *>(this->sender());
    auto index = view->indexAt(pos);
    auto info = this->model->fileInfo(index);
    qDebug() << QString("http://localhost:8080/api/v1/file/%1").arg(info.id());
}
