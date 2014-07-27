#include "task.h"

#include <QtCore/QTimer>
#include <QtCore/QEventLoop>
#include <QtWidgets/QApplication>

Task::Task(QObject *parent) :
	QObject(parent),
	_robot(nullptr),
	_interval(500)
{
}

void Task::setMouseArea(QWidget *view) {
	this->_robot = Robot::create(view);
	this->_robot->moveToThread(qApp->thread());
}

QPoint Task::wait(const QPixmap &target) const {
	auto center = this->_robot->find(target);
	while (center.isNull()) {
		QEventLoop wait;
		QTimer::singleShot(this->_interval, &wait, SLOT(quit()));
		wait.exec();
		center = this->_robot->find(target);
	}
	return center;
}

void Task::click(const QPixmap &target) const {
	auto center = this->wait(target);
	this->_waitForClicked(center);
}

void Task::_waitForClicked(const QPoint &target) const {
	QMetaObject::invokeMethod(this->_robot, "click", Q_ARG(QPoint, target));
	QEventLoop wait;
	wait.connect(this->_robot, SIGNAL(clickFinished()), SLOT(quit()));
	wait.exec();
}
