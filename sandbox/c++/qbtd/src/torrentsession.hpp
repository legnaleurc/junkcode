#ifndef TORRENTSESSION_HPP
#define TORRENTSESSION_HPP

#include <QtCore/QUrl>

#include <memory>

class TorrentSession {
public:
	static void initialize();
	static TorrentSession & instance();

	void addTorrent( const QUrl & url );

private:
	TorrentSession();
	~TorrentSession();
	TorrentSession( const TorrentSession & );
	TorrentSession & operator =( const TorrentSession & );
	class Private;
	friend class Private;
	std::shared_ptr< Private > p_;
};

#endif
