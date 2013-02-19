#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "tcpsocket_linux.h"

typedef struct {
	int fd;
} Private;

void TCPSocket_setSocket_( TCPSocket * socket_, int cfd ) {
	Private * p = ( Private * )socket_->priv;
	p->fd = cfd;
}

TCPSocket * TCPSocket_new( void ) {
	TCPSocket * s = ( TCPSocket * )malloc( sizeof( TCPSocket ) );
	Private * p = ( Private * )malloc( sizeof( Private ) );

	s->priv = p;
	p->fd = -1;
	return s;
}

void TCPSocket_delete( TCPSocket * * socket_ ) {
	Private * p = NULL;

	if( !socket_ || !*socket_ ) {
		return;
	}

	p = ( Private * )( *socket_ )->priv;
	close( p->fd );
	free( p );

	free( *socket_ );
	*socket_ = NULL;
}

int TCPSocket_connect( TCPSocket * socket_, const char * hostName, unsigned short port ) {
	Private * p = ( Private * )socket_->priv;
	int cfd = -1;
	struct sockaddr_in serverAddr;
	int iResult = 0;

	memset( &serverAddr, 0, sizeof( serverAddr ) );
	iResult = inet_pton( AF_INET, hostName, &serverAddr.sin_addr );
	if( iResult < 0 ) {
		return -1;
	}
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons( port );

	cfd = socket( AF_INET, SOCK_STREAM, 0 );
	if( cfd < 0 ) {
		return -1;
	}

	iResult = connect( cfd, ( struct sockaddr * )&serverAddr, sizeof( serverAddr ) );
	if( iResult < 0 ) {
		return -1;
	}

	p->fd = cfd;

	return 0;
}

void TCPSocket_disconnect( TCPSocket * socket_ ) {
	Private * p = ( Private * )socket_->priv;

	close( p->fd );
	p->fd = -1;
}

int TCPSocket_read( TCPSocket * socket_, char * buffer, int length ) {
	Private * p = ( Private * )socket_->priv;
	int iResult = 0;

	iResult = read( p->fd, buffer, length );
	if( iResult > 0 ) {
		return iResult;
	} else if( iResult == 0 ) {
		close( p->fd );
		p->fd = -1;
		return iResult;
	} else {
		close( p->fd );
		p->fd = -1;
		return -1;
	}
}

int TCPSocket_write( TCPSocket * socket_, const char * buffer, int length ) {
	Private * p = ( Private * )socket_->priv;
	int iResult = 0;

	iResult = write( p->fd, buffer, length );
	if( iResult < 0 ) {
		close( p->fd );
		p->fd = -1;
		return -1;
	}
	return 0;
}
