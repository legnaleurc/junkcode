#include "drivesystem_p.h"
#include "drivefileinfo_p.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtCore/QEventLoop>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>
#include <QtCore/QUrlQuery>


namespace {

void deletePrivate (DriveSystemPrivate * d) {
    delete d;
}

}


DriveSystem::DriveSystem()
    : d(new Private, &deletePrivate)
{}


void DriveSystem::setBaseUrl(const QString & baseUrl) {
    d->baseUrl = baseUrl;
}


QList<DriveFileInfo> DriveSystem::list(const QString & idOrPath) const {
    QUrlQuery query;
    query.addQueryItem("id_or_path", idOrPath);
    QUrl url(d->baseUrl + "/api/v1/list");
    url.setQuery(query);
    QNetworkRequest request;
    request.setUrl(url);
    auto reply = d->nam.get(request);

    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    QList<DriveFileInfo> rv;
    auto response = reply->readAll();
    QJsonParseError error;
    auto json = QJsonDocument::fromJson(response, &error);
    if (error.error != QJsonParseError::NoError) {
        qDebug() << error.errorString();
        return rv;
    }
    auto data = json.array().toVariantList();
    for (const auto & info : data) {
        DriveFileInfo fileInfo(new DriveFileInfoPrivate(idOrPath, info.value<QVariantMap>()));
        rv.push_back(fileInfo);
    }
    return rv;
}


DriveSystemPrivate::DriveSystemPrivate()
    : nam()
    , baseUrl()
{}
