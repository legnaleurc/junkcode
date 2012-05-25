#include <QtCore/QCoreApplication>

#include "torrentsession.hpp"

int main( int argc, char * argv[] ) {
	QCoreApplication a( argc, argv );

	TorrentSession::initialize();

	TorrentSession::instance().addTorrent( QUrl::fromLocalFile( argv[1] ) );

	return a.exec();
}
