#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>

#include "networkaccessmanagerproxy.h"
#include "database.h"
#include "eventqueue.h"

class Controller : public QObject
{
	Q_OBJECT
public:
	explicit Controller(QObject *parent = 0);
	virtual ~Controller();

	void setNetworkAccessManager(NetworkAccessManagerProxy * namp);
	void setMouseArea(QWidget * ma);

	void start();
    void stop();

signals:
	void api_port_port();

private slots:
	void _onRequestFinished(const QString & path, const QJsonDocument & json);

private:
	typedef std::function<void (const QJsonDocument &)> ApiCallback;

	void _api_start2(const QJsonDocument & json);
	void _api_port_port(const QJsonDocument & json);

	Database _db;
	NetworkAccessManagerProxy * _namp;
	EventQueue * _eventQueue;
	std::map<QString, ApiCallback> _cb;
};

#endif // CONTROLLER_H
