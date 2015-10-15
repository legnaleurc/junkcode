#ifndef WINROBOT_H
#define WINROBOT_H

#include "robot.h"

class WinRobot : public Robot
{
public:
    WinRobot();

protected:
	virtual void doClick(const QPoint &pos, int msDelay) const;
};

#endif
