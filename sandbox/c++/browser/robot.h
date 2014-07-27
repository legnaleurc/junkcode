#ifndef ROBOT_H
#define ROBOT_H

#include <memory>

#include <QtWidgets/QWidget>

class Robot: public QObject
{
	Q_OBJECT
public:
	static Robot * create(QWidget * widget);

	virtual ~Robot();

	QPoint find(const QPixmap & target) const;

	Q_INVOKABLE void click(const QPoint & pos, int msDelay = 0) const;
	Q_INVOKABLE void click(const QPixmap & target, int msDelay = 0) const;

protected:
	Robot();

	QWidget * getWidget() const;

	virtual void doClick(const QPoint & pos, int msDelay) const;

signals:
	void clickFinished() const;

private:
	QWidget * _widget;
};

#endif // ROBOT_H
