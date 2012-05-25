#include "torrentsession_p.hpp"
#include "inspector.hpp"

#include <QtCore/QtDebug>

std::unique_ptr< TorrentSession, std::function< void ( TorrentSession * ) > > TorrentSession::Private::self;

void TorrentSession::Private::destory( TorrentSession * data ) {
	delete data;
}

TorrentSession::Private::Private():
session_() {
}

void TorrentSession::initialize() {
	if( Private::self != nullptr ) {
		return;
	}
	Private::self.reset( new TorrentSession );
	Private::self.get_deleter() = Private::destory;
}

TorrentSession & TorrentSession::instance() {
	if( Private::self == nullptr ) {
		assert( !"not initialized" );
	}
	return *Private::self;
}

TorrentSession::TorrentSession():
p_( new Private ) {
	// TODO read from configure
	this->p_->session_.listen_on( std::make_pair( 6881, 6889 ) );
}

TorrentSession::~TorrentSession() {
}

void TorrentSession::addTorrent( const QUrl & url ) {
	libtorrent::add_torrent_params p;
	// TODO read from configure
	p.save_path = "./";
	// TODO maybe network url
	p.ti = new libtorrent::torrent_info( url.toLocalFile().toStdWString() );
	libtorrent::torrent_handle th = this->p_->session_.add_torrent( p );
	Inspector * inspector = new Inspector( th );
	inspector->connect( inspector, SIGNAL( finished() ), SLOT( deleteLater() ) );
}
