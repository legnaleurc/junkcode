#include <stdio.h>

int main() {
	char tmp[1024];
	while( fgets( tmp, 1024, stdin ) != NULL ) {
		for( char * c = tmp; *c; ++c ) {
			if( *c == '\n' ) {
				printf( "\n" );
			} else {
				printf( "\\x%02X", ( unsigned char )*c );
			}
		}
	}
	return 0;
}
