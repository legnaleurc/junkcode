#include "inspector.hpp"

#include <QtCore/QTimer>

using libtorrent::torrent_handle;
using libtorrent::torrent_info;

Inspector::Inspector( const torrent_handle & th ): QObject(), th( th ) {
	QTimer * timer = new QTimer( this );
	this->connect( timer, SIGNAL( timeout() ), SLOT( print() ) );
	timer->setInterval( 1000 );
	timer->start();
}

void Inspector::print() {
	std::vector< long > fp;
	this->th.file_progress( fp, torrent_handle::piece_granularity );
	int i = 0;
	for( auto it = fp.begin(); it != fp.end(); ++it ) {
		std::cout << *it * 100 / this->th.get_torrent_info().file_at( i ).size << "%" << std::endl;
		++i;
	}
	if( this->th.is_seed() ) {
		emit this->finished();
	}
}
