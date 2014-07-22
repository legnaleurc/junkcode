#include "eventqueue.h"

#include <QtCore/QEventLoop>
#include <QtCore/QThreadPool>
#include <QtGui/QPixmap>
#include <QtCore/QtDebug>

EventQueue::EventQueue(QObject *parent) :
	QObject(parent), QRunnable(),
	_view(nullptr),
	_mouse(nullptr)
{
    this->setAutoDelete(false);
}

void EventQueue::run() {
	QEventLoop loop;
	loop.connect(this, SIGNAL(stop()), SLOT(quit()));
	Mouse mouse(this->_view);
	this->_mouse = &mouse;
    emit this->started();
	loop.exec();
    emit this->stopped();
}

void EventQueue::waitForStarted() {
    this->_mouse = new Mouse(this->_view);
//    QEventLoop wait;
//    wait.connect(this, SIGNAL(started()), SLOT(quit()));
//    QThreadPool::globalInstance()->start(this);
//    wait.exec();
}

void EventQueue::waitForStopped() {
    this->_mouse->deleteLater();
    this->_mouse = nullptr;
//    QEventLoop wait;
//    wait.connect(this, SIGNAL(stopped()), SLOT(quit()));
//    emit this->stop();
//    wait.exec();
//    this->_mouse = nullptr;
}

void EventQueue::setMouseArea(QWidget *view) {
    this->_view = view;
}

void EventQueue::startMission(int api_deck_id, int api_mission_id) {
    QPixmap p(":/ui/main_menu_button_go.png");
    QMetaObject::invokeMethod(this->_mouse, "click", Q_ARG(QPixmap, p));
    qDebug() << "invoked meta method";
}
