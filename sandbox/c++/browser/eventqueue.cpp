#include "eventqueue.h"

#include <QtCore/QEventLoop>
#include <QtCore/QThreadPool>
#include <QtGui/QPixmap>
#include <QtCore/QtDebug>

EventQueue::EventQueue(QObject *parent) :
	QObject(parent), QRunnable(),
	_view(nullptr),
	_bgTask(nullptr)
{
    this->setAutoDelete(false);
}

void EventQueue::run() {
	QEventLoop loop;
	loop.connect(this, SIGNAL(_stop()), SLOT(quit()));
	Task bgTask;
	this->_bgTask = &bgTask;
	this->_bgTask->setMouseArea(this->_view);
	emit this->_started();
	loop.exec();
	this->_bgTask = nullptr;
	emit this->_stopped();
}

void EventQueue::waitForStarted() {
	QEventLoop wait;
	wait.connect(this, SIGNAL(_started()), SLOT(quit()));
	QThreadPool::globalInstance()->start(this);
	wait.exec();
}

void EventQueue::waitForStopped() {
	QEventLoop wait;
	wait.connect(this, SIGNAL(_stopped()), SLOT(quit()));
	emit this->_stop();
	wait.exec();
}

void EventQueue::setMouseArea(QWidget *view) {
	this->_view = view;
}

void EventQueue::startMission(int api_deck_id, int api_mission_id) {
	;
//	QPixmap p(":/ui/main_menu_button_go.png");
//	QMetaObject::invokeMethod(this->_bgTask, "click", Q_ARG(QPixmap, p));
}
