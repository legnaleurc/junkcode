#include <stdio.h>

#include <WinSock2.h>
#include <WS2tcpip.h>

#include "tcpserver.h"
#include "tcpsocket_win32.h"
#include "network_win32.h"

struct TCPServer {
    SOCKET listenSocket;
};

TCPServer * TCPServer_new (void) {
    TCPServer * s = NULL;

    increaseUseCount();

    s = (TCPServer *)malloc(sizeof(TCPServer));
    s->listenSocket = INVALID_SOCKET;
    return s;
}

void TCPServer_delete (TCPServer * self) {
    if (!self) {
        return;
    }

    closesocket(self->listenSocket );
    free(self);

    decreaseUseCount();
}

int TCPServer_listen (TCPServer * self, const char * hostName, unsigned short port) {
    struct addrinfo * result = NULL;
    struct addrinfo hints;
    SOCKET ListenSocket = INVALID_SOCKET;
    char strPort[6];
    int iResult = 0;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    sprintf_s(strPort, sizeof(strPort), "%u", port);

    iResult = getaddrinfo(hostName, strPort, &hints, &result);
    if (iResult != 0) {
        return -1;
    }

    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
    freeaddrinfo (result);
        return -1;
    }

    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
    closesocket(ListenSocket);
    freeaddrinfo(result);
        return -1;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
    closesocket(ListenSocket);
        return -1;
    }

    self->listenSocket = ListenSocket;

    return 0;
}

void TCPServer_close (TCPServer * self) {
    closesocket(self->listenSocket);
    self->listenSocket = INVALID_SOCKET;
}

TCPSocket * TCPServer_accept (TCPServer * self) {
    TCPSocket * client = NULL;
    SOCKET ClientSocket = INVALID_SOCKET;

    ClientSocket = accept(self->listenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        return NULL;
    }

    client = TCPSocket_new();
    TCPSocket_setSocket_(client, ClientSocket);

    return client;
}
