#ifndef MACROBOT_H
#define MACROBOT_H

#include "robot.h"

class MacRobot : public Robot
{
public:
	MacRobot();

protected:
	virtual void doClick(QWidget *widget, const QPoint &pos, int msDelay) const;
};

#endif // MACROBOT_H
