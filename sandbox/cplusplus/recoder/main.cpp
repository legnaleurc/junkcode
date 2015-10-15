#include "input.hpp"

int func( int a, int b ) {
	xInput << a << b;
	return 0;
}

int main( int argc, char * argv[] ) {
	func( 1, 4 );
	return 0;
}
