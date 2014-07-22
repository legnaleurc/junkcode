#include "networkaccessmanagerproxy.h"

#include <QtNetwork/QNetworkReply>

static const QByteArray BA;

NetworkAccessManagerProxy::NetworkAccessManagerProxy(QObject *parent) :
	QNetworkAccessManager(parent),
	_data()
{
	this->connect(this, SIGNAL(finished(QNetworkReply*)), SLOT(_onReplyFinished(QNetworkReply*)));
}

NetworkAccessManagerProxy::~NetworkAccessManagerProxy() {}

QNetworkReply* NetworkAccessManagerProxy::createRequest(Operation op, const QNetworkRequest& request, QIODevice *outgoingData) {
	auto r = request;
	r.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);
	QNetworkReply* reply = QNetworkAccessManager::createRequest(op, r, outgoingData);
	if (request.url().host() != "125.6.189.135") {
		return reply;
	}
	if (!request.url().path().startsWith("/kcsapi")) {
		return reply;
	}
	auto data = QSharedPointer<QByteArray>(new QByteArray);
	this->_data.insert(std::make_pair(request.url().toString(), data));
	this->connect(reply, SIGNAL(readyRead()), SLOT(_onReadyRead()));
	return reply;
}

void NetworkAccessManagerProxy::_onReadyRead() {
	auto reply = qobject_cast<QNetworkReply *>(this->sender());
	auto it = this->_data.find(reply->request().url().toString());
	if (it == this->_data.end()) {
		return;
	}
	auto data = it->second;
	auto buffer = reply->peek(reply->size());
	data->append(buffer);
}

void NetworkAccessManagerProxy::_onReplyFinished(QNetworkReply *reply) {
//	qDebug() << reply->request().url();
//	this->_debug();
	auto it = this->_data.find(reply->request().url().toString());
	if (it == this->_data.end()) {
		return;
	}
	auto path = reply->request().url().path();
	auto data = it->second;
	this->_data.erase(it);
	auto rd = (*data).mid(7);
	auto json = QJsonDocument::fromJson(rd);
	emit this->requestFinished(path, json);
}

void NetworkAccessManagerProxy::_debug() {
	for (auto p : this->_data) {
		qDebug() << p.first;
//		qDebug() << p.first->request().url();
//		qDebug() << p.first->request().url().toString();
//		qDebug() << p.first->isFinished();
	}
//	std::for_each(this->_data.begin(), this->_data.end(), [](const status) {}});
}
