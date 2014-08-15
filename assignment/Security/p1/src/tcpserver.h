#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "config.h"
#include "tcpsocket.h"

typedef struct TCPServer TCPServer;

DLL_SYMBOL TCPServer * TCPServer_new (void);
DLL_SYMBOL void TCPServer_delete (TCPServer * self);
DLL_SYMBOL int TCPServer_listen (TCPServer * self, const char * hostName, unsigned short port);
DLL_SYMBOL void TCPServer_close (TCPServer * self);
DLL_SYMBOL TCPSocket * TCPServer_accept (TCPServer * self);

#endif
