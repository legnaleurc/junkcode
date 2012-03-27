#include "napplication.hpp"
#include "nwidget.hpp"

int main( int argc, char * argv[] ) {
	NApplication & a = NApplication::instance();
	NWidget w;
	w.keyPressed().connect( [&a]( int keyCode ) {
		if( keyCode == 'q' ) {
			a.quit();
		}
	} );
	return a.exec();
}
