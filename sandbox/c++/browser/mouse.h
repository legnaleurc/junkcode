#ifndef MOUSE_H
#define MOUSE_H

#include <QtCore/QObject>
#include <QtCore/QPoint>

#include "robot.h"

class Mouse: public QObject
{
	Q_OBJECT
public:
	Mouse(QWidget * view);

	QPoint wait(const QPixmap & target) const;
    Q_INVOKABLE void click(const QPixmap & target) const;

private:
	QWidget * _ma;
	QSharedPointer<Robot> _robot;
	int _interval;
};

#endif // MOUSE_H
