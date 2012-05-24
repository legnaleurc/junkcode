#include <QtGui/QApplication>

#include "controller.hpp"

int main( int argc, char * argv[] ) {
	QApplication a( argc, argv );

	Controller c( 1000 );
	a.connect( &c, SIGNAL( finished() ), SLOT( quit() ) );
	c.start();

	return a.exec();
}
