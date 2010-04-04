#include <stdio.h>

int main() {
	for( char tmp[1024]; fgets( tmp, sizeof( tmp ), stdin ) != NULL; ) {
		for( unsigned char * c = tmp; *c; ++c ) {
			if( *c == '\n' ) {
				printf( "\n" );
			} else {
				printf( "\\x%02X", *c );
			}
		}
	}
	return 0;
}
