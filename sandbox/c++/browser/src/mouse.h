#ifndef MOUSE_H
#define MOUSE_H

#include <QtCore/QPoint>

#include "robot.h"

class Mouse
{
public:
	Mouse(QWidget * view);

	QPoint wait(const QPixmap & target) const;
	void click(const QPoint & target) const;

private:
	QWidget * _ma;
	QSharedPointer<Robot> _robot;
	int _interval;
};

#endif // MOUSE_H
