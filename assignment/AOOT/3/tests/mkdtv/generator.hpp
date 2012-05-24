#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include <QtCore/QObject>
#include <QtCore/QRunnable>

class Generator : public QObject, public QRunnable {
	Q_OBJECT
public:
	Generator();

	virtual void run();

signals:
	void finished( const QByteArray & data, const QByteArray & type );
};

#endif // GENERATOR_HPP
