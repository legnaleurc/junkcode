#include <stdio.h>
#include <string.h>

#include "tcpserver.h"

int main(void) {
    char buffer[4096];
    TCPServer * server = TCPServer_new();
    TCPSocket * socket = NULL;

    printf("Please set your password:\n");
    fgets(buffer, sizeof(buffer), stdin);

    TCPServer_listen(server, "127.0.0.1", 9527);

    socket = TCPServer_accept(server);
    TCPSocket_write(socket, buffer, strlen(buffer));

    TCPSocket_disconnect(socket);
    TCPSocket_delete(socket);

    TCPServer_close(server);
    TCPServer_delete(server);

    return 0;
}
