#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "config.h"
#include "tcpsocket.h"

typedef struct {
	void * priv;
} TCPServer;

DLL_SYMBOL TCPServer * TCPServer_new( void );
DLL_SYMBOL void TCPServer_delete( TCPServer * * server );
DLL_SYMBOL int TCPServer_listen( TCPServer * server, const char * hostName, unsigned short port );
DLL_SYMBOL void TCPServer_close( TCPServer * server );
DLL_SYMBOL TCPSocket * TCPServer_accept( TCPServer * server );

#endif
