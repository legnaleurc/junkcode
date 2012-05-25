#include <QtCore/QCoreApplication>

#include <libtorrent/session.hpp>

#include "inspector.hpp"

int main( int argc, char * argv[] ) {
	QCoreApplication a( argc, argv );

	libtorrent::session s;
	s.listen_on( std::make_pair( 6881, 6889 ) );
	libtorrent::add_torrent_params p;
	p.save_path = "./";
	p.ti = new libtorrent::torrent_info( argv[1] );
	libtorrent::torrent_handle th = s.add_torrent( p );

	Inspector inspector( th );
	a.connect( &inspector, SIGNAL( finished() ), SLOT( quit() ) );

	return a.exec();
}
