#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include "config.h"

typedef struct TCPSocket TCPSocket;

DLL_SYMBOL TCPSocket * TCPSocket_new (void);
DLL_SYMBOL void TCPSocket_delete (TCPSocket * self);
DLL_SYMBOL int TCPSocket_connect (TCPSocket * self, const char * hostName, unsigned short port);
DLL_SYMBOL void TCPSocket_disconnect (TCPSocket * self);
DLL_SYMBOL int TCPSocket_read (TCPSocket * self, char * buffer, int length);
DLL_SYMBOL int TCPSocket_write (TCPSocket * self, const char * buffer, int length);

#endif
