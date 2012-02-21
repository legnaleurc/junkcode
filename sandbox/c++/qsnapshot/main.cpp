#include <QtGui/QApplication>
#include "mainwindow.hpp"

int main( int argc, char * argv[] ) {
	QApplication a( argc, argv );
	qsnapshot::widget::MainWindow w;
	w.show();

	return a.exec();
}
