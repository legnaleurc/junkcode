#ifndef TORRENTSESSION_HPP_
#define TORRENTSESSION_HPP_

#include "torrentsession.hpp"

#include <libtorrent/session.hpp>

class TorrentSession::Private {
public:
	static void destory( TorrentSession * );
	static std::unique_ptr< TorrentSession, std::function< void ( TorrentSession * ) > > self;

	Private();

	libtorrent::session session_;
};

#endif
