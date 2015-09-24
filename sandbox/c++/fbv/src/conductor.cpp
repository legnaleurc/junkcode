#include "conductor.hpp"

#include <QtWidgets/QApplication>


using fbv::Conductor;


Conductor::Conductor(QObject * parent):
QObject(parent),
_kh(new fbv::KeyboardHandler(this)),
_container(new QStackedWidget) {
}

void Conductor::prepare() {
    auto rect = qApp->desktop()->availableGeometry();

    QGraphicsScene * scene = new QGraphicsScene(this->_kh);
    QGraphicsView * view = new QGraphicsView(scene, this->_container);

    QLabel * label = new QLabel;
    QMovie * movie = new QMovie("/tmp/test.gif");
    label->setMovie(movie);
    movie->start();

    auto wpItem = scene->addWidget(label);

    this->_container->addWidget(view);
    this->_container->setCurrentIndex(0);

    this->_container->setGeometry(rect);
    this->_container->show();
}
