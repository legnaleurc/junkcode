#ifndef NETWORKACCESSMANAGERPROXY_H
#define NETWORKACCESSMANAGERPROXY_H

#include <QtNetwork/QNetworkAccessManager>

#include <map>
#include <memory>

#include <QtCore/QSharedPointer>
#include <QtCore/QJsonDocument>

class NetworkAccessManagerProxy : public QNetworkAccessManager
{
	Q_OBJECT
public:
	explicit NetworkAccessManagerProxy(QObject *parent = 0);
	virtual ~NetworkAccessManagerProxy();

protected:
	virtual QNetworkReply* createRequest(Operation op, const QNetworkRequest& request, QIODevice *outgoingData);

signals:
	void requestFinished(const QString & path, const QJsonDocument & json);

private slots:
	void _onReadyRead();
	void _onReplyFinished(QNetworkReply * reply);

private:
	void _debug();
	std::map<QString, QSharedPointer<QByteArray>> _data;
};

#endif // NETWORKACCESSMANAGERPROXY_H
