#include "drivesystem_p.h"
#include "drivefileinfo_p.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtCore/QEventLoop>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>
#include <QtCore/QUrlQuery>

#include <queue>


// namespace {

void dumpTree (DriveSystemPrivate * d) {
    std::queue<std::shared_ptr<Node>> q;
    q.push(d->root);

    while (!q.empty()) {
        auto n = q.front();
        q.pop();
        for (const auto & p : n->children) {
            q.push(p.second);
        }
        qDebug() << n->info;
    }
}

// }


DriveSystem::DriveSystem(QObject * parent)
    : QObject(parent)
    , d(new Private(this))
{}


void DriveSystem::setBaseUrl(const QString & baseUrl) {
    d->baseUrl = baseUrl;
    d->socket->open(QUrl("ws://localhost:8000/socket/v1/changes"));

    auto rootInfo = d->fetchInfo("/");
    d->root = std::make_shared<Node>(rootInfo, nullptr);
    d->database.clear();
    d->database.emplace(rootInfo.id(), d->root);
}


DriveFileInfo DriveSystem::info(const QString & idOrPath) const {
    auto fileInfo = d->fetchInfo(idOrPath);
    auto hit = d->database.find(fileInfo.id());
    if (hit != d->database.end()) {
        auto node = hit->second.lock();
        assert(node || !"dead hash");
        if (node->info != fileInfo) {
            if (node->info.parentId() != fileInfo.parentId()) {
                // moved

                // remove from old parent
                auto parentNode = node->parent.lock();
                auto cit = parentNode->children.find(node->info.id());
                if (cit != parentNode->children.end()) {
                    parentNode->children.erase(cit);
                }

                // add to new parent
                hit = d->database.find(fileInfo.parentId());
                assert(hit != d->database.end() || !"invalid parent");
                parentNode = hit->second.lock();
                parentNode->children.emplace(fileInfo.id(), node);
            }
            node->info = fileInfo;
        }
    } else {
        hit = d->database.find(fileInfo.parentId());
        assert(hit != d->database.end());
        auto parentNode = hit->second.lock();
        auto node = std::make_shared<Node>(fileInfo, parentNode);
        parentNode->children.emplace(fileInfo.id(), node);
        d->database.emplace(fileInfo.id(), node);
    }
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
        DriveFileInfo fileInfo(new DriveFileInfoPrivate(info.toMap()));
        rv.push_back(fileInfo);
    }
    return rv;
}


DriveSystemPrivate::DriveSystemPrivate(DriveSystem * parent)
    : QObject(parent)
    , q(parent)
    , baseUrl()
    , socket(new QWebSocket(QString(), QWebSocketProtocol::VersionLatest, this))
    , root()
    , database()
{
    QObject::connect(this->socket, &QWebSocket::textMessageReceived, this, &DriveSystemPrivate::onMessage);
    QObject::connect(this->socket, qOverload<QAbstractSocket::SocketError>(&QWebSocket::error), this, &DriveSystemPrivate::onError);
    QObject::connect(this, &DriveSystemPrivate::removed, q, &DriveSystem::removed);
    QObject::connect(this, &DriveSystemPrivate::updated, q, &DriveSystem::updated);
}


DriveFileInfo DriveSystemPrivate::fetchInfo(const QString & idOrPath) {
    auto data = this->get("/api/v1/info", {
        {"id_or_path", idOrPath},
    });

    if (!data.isValid()) {
        return DriveFileInfo();
    }

    auto mapData = data.toMap();
    DriveFileInfo fileInfo(new DriveFileInfoPrivate(mapData));
    return fileInfo;
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


void DriveSystemPrivate::applyChange(const QVariantMap & change) {
    auto id = change.value("fileId").toString();
    auto removed = change.value("removed").toBool();
    if (removed) {
        emit this->removed(id);
        return;
    }
    auto mapInfo = change.value("file").toMap();
    DriveFileInfo info(new DriveFileInfoPrivate(mapInfo));
}


void DriveSystemPrivate::onMessage(const QString & message) {
    QJsonParseError error;
    auto json = QJsonDocument::fromJson(message.toUtf8(), &error);
    if (error.error != QJsonParseError::NoError) {
        qDebug() << error.errorString();
        return;
    }
    auto listData = json.toVariant().toList();
    for (const auto & data : listData) {
        this->applyChange(data.toMap());
    }
}


void DriveSystemPrivate::onError(QAbstractSocket::SocketError error) {
    qDebug() << "error" << error << this->socket->errorString();
}


Node::Node(const DriveFileInfo & info, std::shared_ptr<Node> parent)
    : info(info)
    , parent(parent)
    , children()
{}
