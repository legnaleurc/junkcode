#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include "config.h"

typedef struct {
	void * priv;
} TCPSocket;

DLL_SYMBOL TCPSocket * TCPSocket_new( void );
DLL_SYMBOL void TCPSocket_delete( TCPSocket * * socket );
DLL_SYMBOL int TCPSocket_connect( TCPSocket * socket, const char * hostName, unsigned short port );
DLL_SYMBOL void TCPSocket_disconnect( TCPSocket * socket );
DLL_SYMBOL int TCPSocket_read( TCPSocket * socket, char * buffer, int length );
DLL_SYMBOL int TCPSocket_write( TCPSocket * socket, const char * buffer, int length );

#endif
