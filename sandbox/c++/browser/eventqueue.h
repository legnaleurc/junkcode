#ifndef EVENTQUEUE_H
#define EVENTQUEUE_H

#include <QObject>
#include <QtCore/QRunnable>

#include "mouse.h"

class EventQueue : public QObject, public QRunnable
{
	Q_OBJECT
public:
	explicit EventQueue(QObject *parent = 0);

	void setMouseArea(QWidget * view);

	virtual void run();

signals:
	void stop();

public slots:
	void startMission(int api_deck_id, int api_mission_id);

private:
	QWidget * _view;
	Mouse * _mouse;
};

#endif // EVENTQUEUE_H
