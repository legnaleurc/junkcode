#include <stdio.h>

#include <WinSock2.h>
#include <WS2tcpip.h>

#include "tcpserver.h"
#include "tcpsocket_win32.h"
#include "network_win32.h"

typedef struct {
	SOCKET listenSocket;
} Private;

TCPServer * TCPServer_new( void ) {
	TCPServer * s = NULL;
	Private * p = NULL;

	increaseUseCount();

	s = ( TCPServer * )malloc( sizeof( TCPServer ) );
	p = ( Private * )malloc( sizeof( Private ) );
	s->priv = p;
	p->listenSocket = INVALID_SOCKET;
	return s;
}

void TCPServer_delete( TCPServer * * server ) {
	Private * p = NULL;

	if( !server || !*server ) {
		return;
	}

	p = ( Private * )( *server )->priv;
	closesocket( p->listenSocket );
	free( p );

	free( *server );
	*server = NULL;

	decreaseUseCount();
}

int TCPServer_listen( TCPServer * server, const char * hostName, unsigned short port ) {
	Private * p = ( Private * )server->priv;
	struct addrinfo * result = NULL;
	struct addrinfo hints;
	SOCKET ListenSocket = INVALID_SOCKET;
	char strPort[6];
	int iResult = 0;

	memset( &hints, 0, sizeof( hints ) );
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	sprintf_s( strPort, sizeof( strPort ), "%u", port );

	iResult = getaddrinfo( hostName, strPort, &hints, &result );
	if( iResult != 0 ) {
		return -1;
	}

	ListenSocket = socket( result->ai_family, result->ai_socktype, result->ai_protocol );
	if( ListenSocket == INVALID_SOCKET ) {
		freeaddrinfo( result );
		return -1;
	}

	iResult = bind( ListenSocket, result->ai_addr, result->ai_addrlen );
	if( iResult == SOCKET_ERROR ) {
		closesocket( ListenSocket );
		freeaddrinfo( result );
		return -1;
	}

	freeaddrinfo( result );

	iResult = listen( ListenSocket, SOMAXCONN );
	if( iResult == SOCKET_ERROR ) {
		closesocket( ListenSocket );
		return -1;
	}

	p->listenSocket = ListenSocket;

	return 0;
}

void TCPServer_close( TCPServer * server ) {
	Private * p = ( Private * )server->priv;

	closesocket( p->listenSocket );
	p->listenSocket = INVALID_SOCKET;
}

TCPSocket * TCPServer_accept( TCPServer * server ) {
	Private * p = ( Private * )server->priv;
	TCPSocket * client = NULL;
	SOCKET ClientSocket = INVALID_SOCKET;

	ClientSocket = accept( p->listenSocket, NULL, NULL );
	if( ClientSocket == INVALID_SOCKET ) {
		return NULL;
	}

	client = TCPSocket_new();
	TCPSocket_setSocket_( client, ClientSocket );

	return client;
}
