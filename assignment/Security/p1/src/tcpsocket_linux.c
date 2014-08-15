#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "tcpsocket_linux.h"

struct TCPSocket {
    int fd;
};

TCPSocket * TCPSocket_new (void) {
    TCPSocket * s = (TCPSocket *)malloc(sizeof(TCPSocket));
    s->fd = -1;
    return s;
}

void TCPSocket_delete (TCPSocket * self) {
    if (!self) {
        return;
    }
    close(self->fd);
    free(self);
}

int TCPSocket_connect (TCPSocket * self, const char * hostName, unsigned short port) {
    int cfd = -1;
    struct sockaddr_in serverAddr;
    int iResult = 0;

    memset(&serverAddr, 0, sizeof(serverAddr));
    iResult = inet_pton(AF_INET, hostName, &serverAddr.sin_addr);
    if (iResult < 0) {
        return -1;
    }
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);

    cfd = socket(AF_INET, SOCK_STREAM, 0);
    if (cfd < 0) {
        return -1;
    }

    iResult = connect(cfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    if (iResult < 0) {
        return -1;
    }

    self->fd = cfd;

    return 0;
}

void TCPSocket_disconnect (TCPSocket * self) {
    close(self->fd);
    self->fd = -1;
}

int TCPSocket_read (TCPSocket * self, char * buffer, int length) {
    int iResult = read(self->fd, buffer, length);
    if( iResult > 0 ) {
        return iResult;
    } else if( iResult == 0 ) {
        close(self->fd);
        self->fd = -1;
        return iResult;
    } else {
        close(self->fd);
        self->fd = -1;
        return -1;
    }
}

int TCPSocket_write (TCPSocket * self, const char * buffer, int length) {
    int iResult = write(self->fd, buffer, length);
    if (iResult < 0) {
        close(self->fd);
        self->fd = -1;
        return -1;
    }
    return 0;
}

void TCPSocket_setSocket_ (TCPSocket * self, int cfd) {
    self->fd = cfd;
}
