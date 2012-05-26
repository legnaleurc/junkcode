#ifndef CONTROLSESSION_HPP_
#define CONTROLSESSION_HPP_

#include "controlsession.hpp"

class ControlSession::Private: public QObject {
	Q_OBJECT
public:
	explicit Private( QLocalSocket * socket );

public slots:
	void onDisconnected();

signals:
	void disconnected();

public:
	QLocalSocket * socket;
};

#endif
