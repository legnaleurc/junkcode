#include "mainwindow.hpp"

#include <QtGui/QApplication>

int main( int argv, char * args[] ) {
	QApplication app( argv, args );

	MainWindow mainWindow;
	mainWindow.setGeometry( 100, 100, 800, 500 );
	mainWindow.show();

	return app.exec();
}
