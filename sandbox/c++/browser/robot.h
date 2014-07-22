#ifndef ROBOT_H
#define ROBOT_H

#include <QtCore/QSharedPointer>

class Robot
{
public:
	static QSharedPointer<Robot> create();

	virtual ~Robot();

	QPoint find(QWidget * widget, const QPixmap & target) const;

	void click(QWidget * widget, const QPoint & pos, int msDelay = 0) const;
	void click(QWidget * widget, const QPixmap & target, int msDelay = 0) const;

protected:
	Robot();

	virtual void doClick(QWidget * widget, const QPoint & pos, int msDelay) const;
};

#endif // ROBOT_H
