#include "mouse.h"

#include <cassert>

#include <QtCore/QEventLoop>
#include <QtCore/QTimer>
#include <QtCore/QThread>
#include <QtWidgets/QApplication>
#include <QtCore/QtDebug>

Mouse::Mouse(QWidget *view):
	_ma(view),
	_robot(Robot::create(view)),
	_interval(500)
{
}

QPoint Mouse::wait(const QPixmap &target) const {
	auto center = this->_robot->find(this->_ma, target);
	while (center.isNull()) {
		QEventLoop wait;
		QTimer::singleShot(this->_interval, &wait, SLOT(quit()));
		wait.exec();
		center = this->_robot->find(this->_ma, target);
	}
	return center;
}

void Mouse::click(const QPoint &target) const {
    assert(qApp->thread() == QThread::currentThread() || !"must run on the main thread");

	this->_robot->click(this->_ma, target);
}
