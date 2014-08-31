#include "macrobot.h"

#include <QtWidgets/QWidget>
#include <QtCore/QEventLoop>
#include <QtCore/QTimer>
#include <QtCore/QtDebug>

#include <ApplicationServices/ApplicationServices.h>

MacRobot::MacRobot()
{
}

void MacRobot::doClick(const QPoint &pos, int msDelay) const {
	auto global = this->getWidget()->mapToGlobal(pos);
	qDebug() << global;

	CGEventRef down = CGEventCreateMouseEvent(NULL, kCGEventLeftMouseDown, CGPointMake(global.x(), global.y()), kCGMouseButtonLeft);
	CGEventRef up = CGEventCreateMouseEvent(NULL, kCGEventLeftMouseUp, CGPointMake(global.x(), global.y()), kCGMouseButtonLeft);

	if (msDelay > 0) {
		QEventLoop wait;
		QTimer::singleShot(msDelay, &wait, SLOT(quit()));
		wait.exec();
	}
	CGEventPost(kCGHIDEventTap, down);
	if (msDelay > 0) {
		QEventLoop wait;
		QTimer::singleShot(msDelay, &wait, SLOT(quit()));
		wait.exec();
	}
	CGEventPost(kCGHIDEventTap, up);

	CFRelease(down);
	CFRelease(up);
}
