#ifndef CONTROLSESSION_HPP
#define CONTROLSESSION_HPP

#include <QtNetwork/QLocalServer>

#include <memory>

class ControlSession: public QObject {
	Q_OBJECT
public:
	explicit ControlSession( QLocalSocket * socket );

	void close();

signals:
	void disconnected();

private:
	class Private;
	std::shared_ptr< Private > p_;
};

#endif
