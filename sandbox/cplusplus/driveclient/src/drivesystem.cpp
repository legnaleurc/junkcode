#include "drivesystem_p.h"
#include "drivefileinfo_p.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtCore/QEventLoop>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>
#include <QtCore/QUrlQuery>


DriveSystem::DriveSystem(QObject * parent)
    : QObject(parent)
    , d(new Private(this))
{}


void DriveSystem::setBaseUrl(const QString & baseUrl) {
    d->baseUrl = baseUrl;
    d->socket->open(QUrl("ws://localhost:8000/socket/v1/changes"));
}


DriveFileInfo DriveSystem::info(const QString & idOrPath) const {
    auto data = d->get("/api/v1/info", {
        {"id_or_path", idOrPath},
    });

    if (!data.isValid()) {
        return DriveFileInfo();
    }

    auto mapData = data.toMap();
    DriveFileInfo fileInfo(new DriveFileInfoPrivate(this, mapData));
    return fileInfo;
}


QList<DriveFileInfo> DriveSystem::list(const QString & idOrPath) const {
    auto data = d->get("/api/v1/list", {
        {"id_or_path", idOrPath},
    });

    QList<DriveFileInfo> rv;
    if (!data.isValid()) {
        return rv;
    }

    auto listData = data.toList();
    for (const auto & info : listData) {
        DriveFileInfo fileInfo(new DriveFileInfoPrivate(this, info.toMap()));
        rv.push_back(fileInfo);
    }
    return rv;
}


DriveSystemPrivate::DriveSystemPrivate(DriveSystem * parent)
    : QObject(parent)
    , q(parent)
    , baseUrl()
    , socket(new QWebSocket(QString(), QWebSocketProtocol::VersionLatest, this))
{
    QObject::connect(this->socket, &QWebSocket::textMessageReceived, this, &DriveSystemPrivate::onMessage);
    QObject::connect(this->socket, qOverload<QAbstractSocket::SocketError>(&QWebSocket::error), this, &DriveSystemPrivate::onError);
}


QVariant DriveSystemPrivate::get(const QString & path, const QList<QPair<QString, QString>> & params) {
    QUrlQuery query;
    for (const auto & p : params) {
        query.addQueryItem(p.first, p.second);
    }
    QUrl url(this->baseUrl + path);
    url.setQuery(query);
    QNetworkRequest request;
    request.setUrl(url);
    QNetworkAccessManager nam;
    auto reply = nam.get(request);

    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    auto response = reply->readAll();
    reply->deleteLater();
    QJsonParseError error;
    auto json = QJsonDocument::fromJson(response, &error);
    if (error.error != QJsonParseError::NoError) {
        qDebug() << error.errorString();
        return QVariant();
    }
    return json.toVariant();
}


void DriveSystemPrivate::onMessage(const QString & message) {
    QJsonParseError error;
    auto json = QJsonDocument::fromJson(message.toUtf8(), &error);
    if (error.error != QJsonParseError::NoError) {
        qDebug() << error.errorString();
        return;
    }
    auto data = json.toVariant().toList();
    qDebug() << data;
}


void DriveSystemPrivate::onError(QAbstractSocket::SocketError error) {
    qDebug() << "error" << error << this->socket->errorString();
}
