#ifndef ROBOT_H
#define ROBOT_H

#include <memory>

#include <QtWidgets/QWidget>

class Robot
{
public:
    static std::shared_ptr<Robot> create(QWidget * widget);

	virtual ~Robot();

	QPoint find(const QPixmap & target) const;

    void click(const QPoint & pos, int msDelay = 0) const;
    void click(const QPixmap & target, int msDelay = 0) const;
    void moveBy(int x, int y) const;

protected:
	Robot();

	QWidget * getWidget() const;

    virtual void doClick(const QPoint & pos, int msDelay) const;

private:
	QWidget * _widget;
};

#endif // ROBOT_H
