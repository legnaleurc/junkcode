#include "napplication.hpp"
#include "nwidget.hpp"

int main( int argc, char * argv[] ) {
	NApplication & a = NApplication::instance();
	NWidget w;
	return a.exec();
}
