#include <stdio.h>

int main() {
	char c;
	char flag = 1;
	
	while( ( c = getchar() ) != EOF ) {
		if( c == '\"' ) {
			if( flag ) {
				printf( "``" );
			} else {
				printf( "\'\'" );
			}
			flag = !flag;
		} else {
			putchar( c );
		}
	}
	
	return 0;
}
