#include <stdio.h>

#include <WinSock2.h>
#include <WS2tcpip.h>

#include "tcpsocket_win32.h"
#include "network_win32.h"

typedef struct {
	SOCKET socket;
} Private;

void TCPSocket_setSocket_( TCPSocket * socket_, SOCKET clientSocket ) {
	Private * p = ( Private * )socket_->priv;
	p->socket = clientSocket;
}

TCPSocket * TCPSocket_new( void ) {
	TCPSocket * s = ( TCPSocket * )malloc( sizeof( TCPSocket ) );
	Private * p = ( Private * )malloc( sizeof( Private ) );

	increaseUseCount();

	s->priv = p;
	p->socket = INVALID_SOCKET;
	return s;
}

void TCPSocket_delete( TCPSocket * * socket_ ) {
	Private * p = NULL;

	if( !socket_ || !*socket_ ) {
		return;
	}

	p = ( Private * )( *socket_ )->priv;
	closesocket( p->socket );
	free( p );

	free( *socket_ );
	*socket_ = NULL;

	decreaseUseCount();
}

int TCPSocket_connect( TCPSocket * socket_, const char * hostName, unsigned short port ) {
	Private * p = ( Private * )socket_->priv;
	SOCKET ClientSocket = INVALID_SOCKET;
	struct addrinfo * result = NULL;
	struct addrinfo hints;
	char strPort[6];
	int iResult = 0;

	memset( &hints, 0, sizeof( hints ) );
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	sprintf_s( strPort, sizeof( strPort ), "%u", port );
	iResult = getaddrinfo( hostName, strPort, &hints, &result );
	if( iResult != 0 ) {
		return -1;
	}

	ClientSocket = socket( result->ai_family, result->ai_socktype, result->ai_protocol );
	if( ClientSocket == INVALID_SOCKET ) {
		freeaddrinfo( result );
		return -1;
	}

	iResult = connect( ClientSocket, result->ai_addr, result->ai_addrlen );
	if( iResult == SOCKET_ERROR ) {
		freeaddrinfo( result );
		closesocket( ClientSocket );
		return -1;
	}

	freeaddrinfo( result );

	p->socket = ClientSocket;

	return 0;
}

void TCPSocket_disconnect( TCPSocket * socket_ ) {
	Private * p = ( Private * )socket_->priv;

	closesocket( p->socket );
	p->socket = INVALID_SOCKET;
}

int TCPSocket_read( TCPSocket * socket_, char * buffer, int length ) {
	Private * p = ( Private * )socket_->priv;
	int iResult = 0;

	iResult = recv( p->socket, buffer, length, 0 );
	if( iResult > 0 ) {
		return iResult;
	} else if( iResult == 0 ) {
		closesocket( p->socket );
		p->socket = INVALID_SOCKET;
		return iResult;
	} else {
		closesocket( p->socket );
		p->socket = INVALID_SOCKET;
		return -1;
	}
}

int TCPSocket_write( TCPSocket * socket_, const char * buffer, int length ) {
	Private * p = ( Private * )socket_->priv;
	int iResult = 0;

	iResult = send( p->socket, buffer, length, 0 );
	if( iResult == SOCKET_ERROR ) {
		closesocket( p->socket );
		p->socket = INVALID_SOCKET;
		return -1;
	}
	return 0;
}
