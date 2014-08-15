#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "tcpserver.h"
#include "tcpsocket_linux.h"

struct TCPServer {
    int listenFD;
};

TCPServer * TCPServer_new (void) {
    TCPServer * s = (TCPServer *)malloc(sizeof(TCPServer));
    s->listenFD = -1;
    return s;
}

void TCPServer_delete (TCPServer * self) {
    if( !self ) {
        return;
    }

    close(self->listenFD);
    free(self);
}

int TCPServer_listen (TCPServer * self, const char * hostName, unsigned short port) {
    struct sockaddr_in serverAddr;
    int sfd = -1;
    int iResult = 0;

    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd < 0) {
        return -1;
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);

    iResult = inet_pton(AF_INET, hostName, &serverAddr.sin_addr);
    if (iResult < 0) {
        return -1;
    }

    iResult = bind(sfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    if (iResult < 0) {
        return -1;
    }

    iResult = listen(sfd, 10);
    if (iResult < 0) {
        return -1;
    }

    self->listenFD = sfd;

    return 0;
}

void TCPServer_close (TCPServer * self) {
    close(self->listenFD);
    self->listenFD = -1;
}

TCPSocket * TCPServer_accept (TCPServer * self) {
    TCPSocket * client = NULL;
    int cfd = -1;

    cfd = accept(self->listenFD, NULL, NULL);
    if (cfd < 0) {
        return NULL;
    }

    client = TCPSocket_new();
    TCPSocket_setSocket_(client, cfd);

    return client;
}
