#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include <QtCore/QMutex>
#include <QtCore/QFile>
#include <QtGui/QProgressDialog>

class Controller : public QProgressDialog
{
	Q_OBJECT
public:
	explicit Controller( int jobs );

	void start();
	
signals:
	void finished();

private slots:
	void onFinished( const QByteArray &, const QByteArray & );

private:
	QFile output_;
	int jobs_;
	int completed_;
	QMutex mutex_;
	qint64 totalSize_;
};

#endif // CONTROLLER_HPP
