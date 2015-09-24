#include "conductor.hpp"

#include <QtWidgets/QApplication>


using fbv::Conductor;


Conductor::Conductor(QObject * parent):
QObject(parent),
_kh(new fbv::KeyboardHandler(this)),
_container(new QStackedWidget) {
}

void Conductor::prepare() {
    this->connect(this->_kh, SIGNAL(up()), SLOT(_onUp()));
    this->connect(this->_kh, SIGNAL(down()), SLOT(_onDown()));
    this->connect(this->_kh, SIGNAL(left()), SLOT(_onLeft()));
    this->connect(this->_kh, SIGNAL(right()), SLOT(_onRight()));
    this->connect(this->_kh, SIGNAL(q()), SLOT(_onQ()));

    auto rect = qApp->desktop()->availableGeometry();

    QGraphicsScene * scene = new QGraphicsScene(this->_kh);
    QGraphicsView * view = new QGraphicsView(scene, this->_container);

    QLabel * label = new QLabel;
    QMovie * movie = new QMovie("/tmp/1.gif");
    label->setMovie(movie);
    movie->start();

    auto wpItem = scene->addWidget(label);

    this->_container->addWidget(view);
    this->_container->setCurrentIndex(0);

    this->_container->setGeometry(rect);
    this->_container->show();
}

void Conductor::_onUp() {
    qDebug() << "up";
}

void Conductor::_onDown() {
    qDebug() << "down";
}

void Conductor::_onLeft() {
    qDebug() << "left";
}

void Conductor::_onRight() {
    qDebug() << "right";
}

void Conductor::_onQ() {
    this->_container->close();
}
