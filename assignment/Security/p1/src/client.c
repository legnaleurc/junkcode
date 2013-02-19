#include <stdio.h>
#include <string.h>

#include "utility.h"
#include "tcpsocket.h"

int main( void ) {
	char buffer[4096];
	char password[4096];
	TCPSocket * socket = TCPSocket_new();
	int passwordLength = 0;

	TCPSocket_connect( socket, "127.0.0.1", 9527 );
	passwordLength = TCPSocket_read( socket, password, sizeof( password ) );
	TCPSocket_disconnect( socket );
	TCPSocket_delete( &socket );

	password[passwordLength] = '\0';

	printf( "Please type your password:\n" );
	fgets( buffer, sizeof( buffer ), stdin );

	if( strcmp( buffer, password ) == 0 ) {
		printMessage();
		blockByMilliseconds( 2 * 60 * 1000 );
	}

	deleteSelf();

	return 0;
}
