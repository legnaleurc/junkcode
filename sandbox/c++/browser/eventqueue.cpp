#include "eventqueue.h"

#include <QtCore/QEventLoop>

EventQueue::EventQueue(QObject *parent) :
	QObject(parent), QRunnable(),
	_view(nullptr),
	_mouse(nullptr)
{
}

void EventQueue::setMouseArea(QWidget *view) {
	this->_view = view;
}

void EventQueue::run() {
	QEventLoop loop;
	loop.connect(this, SIGNAL(stop()), SLOT(quit()));
	Mouse mouse(this->_view);
	this->_mouse = &mouse;
	loop.exec();
}

void EventQueue::startMission(int api_deck_id, int api_mission_id) {
}
