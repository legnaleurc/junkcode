#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "tcpserver.h"
#include "tcpsocket_linux.h"

typedef struct {
	int listenFD;
} Private;

TCPServer * TCPServer_new( void ) {
	TCPServer * s = NULL;
	Private * p = NULL;

	s = ( TCPServer * )malloc( sizeof( TCPServer ) );
	p = ( Private * )malloc( sizeof( Private ) );
	s->priv = p;
	p->listenFD = -1;
	return s;
}

void TCPServer_delete( TCPServer * * server ) {
	Private * p = NULL;

	if( !server || !*server ) {
		return;
	}

	p = ( Private * )( *server )->priv;
	close( p->listenFD );
	free( p );

	free( *server );
	*server = NULL;
}

int TCPServer_listen( TCPServer * server, const char * hostName, unsigned short port ) {
	Private * p = ( Private * )server->priv;
	struct sockaddr_in serverAddr;
	int sfd = -1;
	int iResult = 0;

	sfd = socket( AF_INET, SOCK_STREAM, 0 );
	if( sfd < 0 ) {
		return -1;
	}

	memset( &serverAddr, 0, sizeof( serverAddr ) );
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons( port );

	iResult = inet_pton( AF_INET, hostName, &serverAddr.sin_addr );
	if( iResult < 0 ) {
		return -1;
	}

	iResult = bind( sfd, ( struct sockaddr * )&serverAddr, sizeof( serverAddr ) );
	if( iResult < 0 ) {
		return -1;
	}

	iResult = listen( sfd, 10 );
	if( iResult < 0 ) {
		return -1;
	}

	p->listenFD = sfd;

	return 0;
}

void TCPServer_close( TCPServer * server ) {
	Private * p = ( Private * )server->priv;

	close( p->listenFD );
	p->listenFD = -1;
}

TCPSocket * TCPServer_accept( TCPServer * server ) {
	Private * p = ( Private * )server->priv;
	TCPSocket * client = NULL;
	int cfd = -1;

	cfd = accept( p->listenFD, NULL, NULL );
	if( cfd < 0 ) {
		return NULL;
	}

	client = TCPSocket_new();
	TCPSocket_setSocket_( client, cfd );

	return client;
}
