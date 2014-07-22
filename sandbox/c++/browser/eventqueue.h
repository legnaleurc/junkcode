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

    virtual void run();

    void setMouseArea(QWidget * view);

    void waitForStarted();
    void waitForStopped();

signals:
	void stop();
    void started();
    void stopped();

public slots:
	void startMission(int api_deck_id, int api_mission_id);

private:
	QWidget * _view;
	Mouse * _mouse;
};

#endif // EVENTQUEUE_H
