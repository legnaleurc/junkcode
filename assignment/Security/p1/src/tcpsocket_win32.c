#include <stdio.h>

#include <WinSock2.h>
#include <WS2tcpip.h>

#include "tcpsocket_win32.h"
#include "network_win32.h"

struct TCPSocket {
    SOCKET socket;
};

TCPSocket * TCPSocket_new (void) {
    TCPSocket * s = (TCPSocket *)malloc(sizeof(TCPSocket));

    increaseUseCount();

    s->socket = INVALID_SOCKET;
    return s;
}

void TCPSocket_delete (TCPSocket * self) {
    if (!self) {
	    return;
    }

    closesocket(self->socket);
    free(self);

    decreaseUseCount();
}

int TCPSocket_connect (TCPSocket * self, const char * hostName, unsigned short port) {
    SOCKET ClientSocket = INVALID_SOCKET;
    struct addrinfo * result = NULL;
    struct addrinfo hints;
    char strPort[6];
    int iResult = 0;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    sprintf_s(strPort, sizeof(strPort), "%u", port);
    iResult = getaddrinfo(hostName, strPort, &hints, &result);
    if (iResult != 0) {
        return -1;
    }

    ClientSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ClientSocket == INVALID_SOCKET) {
        freeaddrinfo(result);
        return -1;
    }

    iResult = connect(ClientSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        freeaddrinfo(result);
        closesocket(ClientSocket);
        return -1;
    }

    freeaddrinfo(result);

    self->socket = ClientSocket;

    return 0;
}

void TCPSocket_disconnect (TCPSocket * self) {
    closesocket(self->socket);
    self->socket = INVALID_SOCKET;
}

int TCPSocket_read (TCPSocket * self, char * buffer, int length) {
    int iResult = recv(self->socket, buffer, length, 0);
    if (iResult > 0) {
        return iResult;
    } else if (iResult == 0) {
        closesocket(self->socket);
        self->socket = INVALID_SOCKET;
        return iResult;
    } else {
        closesocket(self->socket);
        self->socket = INVALID_SOCKET;
        return -1;
    }
}

int TCPSocket_write (TCPSocket * self, const char * buffer, int length) {
    int iResult = send(self->socket, buffer, length, 0);
    if (iResult == SOCKET_ERROR) {
        closesocket(self->socket);
        self->socket = INVALID_SOCKET;
        return -1;
    }
    return 0;
}

void TCPSocket_setSocket_ (TCPSocket * self, SOCKET clientSocket) {
    self->socket = clientSocket;
}
