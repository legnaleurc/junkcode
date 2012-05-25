#ifndef INSPECTOR_HPP
#define INSPECTOR_HPP

#include <QtCore/QObject>
#include <libtorrent/torrent_handle.hpp>

class Inspector : public QObject {
	Q_OBJECT
public:
	explicit Inspector( const libtorrent::torrent_handle & th );

signals:
	void finished();

private slots:
	void print();

private:
	libtorrent::torrent_handle th;
};

#endif
