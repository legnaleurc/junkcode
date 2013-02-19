#include <assert.h>

#include <WinSock2.h>

#include "network_win32.h"

static int WS2Counter = 0;

static void WS2Initialize( void ) {
	WSADATA wsaData;
	int iResult = 0;
	iResult = WSAStartup( MAKEWORD( 2, 2 ), &wsaData );
	assert( iResult == 0 );
}

void increaseUseCount( void ) {
	if( WS2Counter <= 0 ) {
		WS2Initialize();
	}
	++WS2Counter;
}

void decreaseUseCount( void ) {
	--WS2Counter;
	if( WS2Counter <= 0 ) {
		WSACleanup();
	}
}
