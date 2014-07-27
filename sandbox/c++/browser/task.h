#ifndef TASK_H
#define TASK_H

#include <QObject>

#include "robot.h"

class Task : public QObject
{
	Q_OBJECT
public:
	explicit Task(QObject *parent = 0);

	void setMouseArea(QWidget * view);

	QPoint wait(const QPixmap & target) const;
	Q_INVOKABLE void click(const QPixmap & target) const;

signals:

public slots:

private:
	void _waitForClicked(const QPoint & target) const;

	Robot * _robot;
	int _interval;
};

#endif // TASK_H
