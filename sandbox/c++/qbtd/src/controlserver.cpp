#include "controlserver_p.hpp"

#include <QtCore/QtDebug>

#include <cassert>

std::unique_ptr< ControlServer, std::function< void ( ControlServer * ) > > ControlServer::Private::self;

void ControlServer::Private::destory( ControlServer * data ) {
	delete data;
}

// TODO local socket, ipv4, ipv6 version
ControlServer::Private::Private():
server(),
sessions() {
	this->connect( &this->server, SIGNAL( newConnection() ), SLOT( onNewConnection() ) );
}

void ControlServer::Private::onNewConnection() {
	while( this->server.hasPendingConnections() ) {
		QLocalSocket * socket = this->server.nextPendingConnection();
		std::shared_ptr< ControlSession > session( new ControlSession( socket ), []( ControlSession * data )->void {
			QMetaObject::invokeMethod( data, "deleteLater" );
		} );
		this->sessions.push_back( session );
	}
}

void ControlServer::Private::onSessionDisconnected() {
	ControlSession * session = static_cast< ControlSession * >( this->sender() );
	auto it = std::remove_if( this->sessions.begin(), this->sessions.end(), [session]( decltype( this->sessions[0] ) s )->bool {
		return session == s.get();
	} );
	this->sessions.erase( it, this->sessions.end() );
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
	// stop listen control session
	this->p_->server.close();
	// disconnect all session
	for( auto it = this->p_->sessions.begin(); it != this->p_->sessions.end(); ++it ) {
		( *it )->close();
	}
}
