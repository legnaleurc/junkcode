#ifndef TCPSOCKET_WIN32_H
#define TCPSOCKET_WIN32_H

#include <WinSock2.h>

#include "tcpsocket.h"

void TCPSocket_setSocket_( TCPSocket * socket_, SOCKET clientSocket );

#endif
