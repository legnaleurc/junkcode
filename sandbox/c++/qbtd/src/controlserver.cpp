#include "controlserver_p.hpp"

#include <QtCore/QtDebug>

#include <cassert>

std::unique_ptr< ControlServer, std::function< void ( ControlServer * ) > > ControlServer::Private::self;

void ControlServer::Private::destory( ControlServer * data ) {
	delete data;
}

// TODO local socket, ipv4, ipv6 version
ControlServer::Private::Private():
server() {
	this->connect( &this->server, SIGNAL( newConnection() ), SLOT( onNewConnection() ) );
}

void ControlServer::Private::onNewConnection() {
	while( this->server.hasPendingConnections() ) {
		QLocalSocket * socket = this->server.nextPendingConnection();
		// TODO open a control session
	}
}

void ControlServer::initialize() {
	if( Private::self ) {
		return;
	}
	Private::self.reset( new ControlServer );
	Private::self.get_deleter() = Private::destory;
}

ControlServer & ControlServer::instance() {
	if( !Private::self ) {
		assert( !"not initialized" );
	}
	return *Private::self;
}

ControlServer::ControlServer():
p_( new Private ) {
	this->p_->server.listen( "qbtd" );
}

ControlServer::~ControlServer() {
	this->p_->server.close();
}
