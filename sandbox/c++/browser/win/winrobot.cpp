#include "winrobot.h"

#include <QtWidgets/QWidget>
#include <QtCore/QEventLoop>
#include <QtCore/QTimer>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDesktopWidget>
#include <QtCore/QtDebug>

#include <windows.h>

WinRobot::WinRobot()
{
}

void WinRobot::doClick(const QPoint &pos, int msDelay) const {
	auto global = this->getWidget()->mapToGlobal(pos);
	qDebug() << global;

    QApplication::desktop()->cursor().setPos(global);

	if (msDelay > 0) {
		QEventLoop wait;
		QTimer::singleShot(msDelay, &wait, SLOT(quit()));
		wait.exec();
    }

    mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);

	if (msDelay > 0) {
		QEventLoop wait;
		QTimer::singleShot(msDelay, &wait, SLOT(quit()));
		wait.exec();
	}
}
