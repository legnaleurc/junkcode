#ifndef CONTROLSERVER_HPP_
#define CONTROLSERVER_HPP_

#include "controlserver.hpp"

#include <QtNetwork/QLocalServer>

class ControlServer::Private : public QObject {
	Q_OBJECT
public:
	static std::unique_ptr< ControlServer, std::function< void ( ControlServer * ) > > self;
	static void destory( ControlServer * );

	Private();

public slots:
	void onNewConnection();

public:
	QLocalServer server;
};

#endif
