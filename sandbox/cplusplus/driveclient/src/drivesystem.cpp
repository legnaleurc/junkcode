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
    std::queue<DriveNodeSP> q;
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


DriveFileInfo DriveSystem::setBaseUrl(const QString & host, int port) {
    d->baseUrl.setHost(host);
    d->baseUrl.setPort(port);
    d->socket->open(d->createSocketUrl());

    auto rootInfo = d->fetchInfo("/");
    d->root = d->createNode(rootInfo, nullptr);
    d->database.clear();
    d->database.emplace(rootInfo.id(), d->root);

    return rootInfo;
}


DriveNodeSP DriveSystem::deleteNode(const QString & id) {
    auto wit = d->database.find(id);
    if (wit == d->database.end()) {
        return nullptr;
    }
    auto node = wit->second.lock();
    if (!node) {
        return nullptr;
    }
    // should not delete root node
    auto parentNode = node->parent.lock();
    if (!parentNode) {
        return nullptr;
    }
    auto sit = parentNode->children.find(id);
    if (sit == parentNode->children.end()) {
        return nullptr;
    }
    d->database.erase(wit);
    parentNode->children.erase(sit);
    return node;
}


DriveNodeSP DriveSystem::upsertNode(const DriveFileInfo & info) {
    d->upsertNode(info);
    return this->node(info.id());
}


DriveNodeSP DriveSystem::node(const QString & id) const {
    auto wit = d->database.find(id);
    if (wit == d->database.end()) {
        return nullptr;
    }
    auto node = wit->second.lock();
    assert(node);
    return node;
}


DriveNodeSP DriveSystem::root() const {
    return d->root;
}


DriveFileInfo DriveSystem::info(const QString & idOrPath) const {
    auto fileInfo = d->fetchInfo(idOrPath);
    // d->upsertNode(fileInfo);
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
        // d->upsertNode(fileInfo);
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
    QObject::connect(this, &DriveSystemPrivate::directoryUpdated, q, &DriveSystem::directoryUpdated);
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
    auto url = this->createApiUrl(path);
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


void DriveSystemPrivate::upsertNode(const DriveFileInfo & fileInfo) {
    auto hit = this->database.find(fileInfo.id());
    if (hit != this->database.end()) {
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
                hit = this->database.find(fileInfo.parentId());
                assert(hit != this->database.end() || !"invalid parent");
                parentNode = hit->second.lock();
                node->parent = parentNode;
                parentNode->children.emplace(fileInfo.id(), node);
            }
            node->info = fileInfo;
        }
    } else {
        hit = this->database.find(fileInfo.parentId());
        if (hit != this->database.end()) {
            auto parentNode = hit->second.lock();
            auto node = this->createNode(fileInfo, parentNode);
            parentNode->children.emplace(fileInfo.id(), node);
            this->database.emplace(fileInfo.id(), node);
        }
    }
}


void DriveSystemPrivate::applyChange(const QVariantMap & change) {
    auto removed = change.value("removed").toBool();
    auto data = change;
    if (!removed) {
        data = data.value("node").toMap();
    }
    auto id = data.value("id").toString();
    auto node = q->node(id);
    if (!node) {
        // not populated yet, no action needed
        return;
    }
    auto parent = node->parent.lock();
    // node is probably the root node
    if (parent) {
        emit this->directoryUpdated(parent->info.id());
    }
    // TODO emit single file change
    return;
}


void DriveSystemPrivate::onMessage(const QString & message) {
    QJsonParseError error;
    auto json = QJsonDocument::fromJson(message.toUtf8(), &error);
    if (error.error != QJsonParseError::NoError) {
        qDebug() << error.errorString();
        return;
    }
    auto mapData = json.toVariant().toMap();
    this->applyChange(mapData);
}


void DriveSystemPrivate::onError(QAbstractSocket::SocketError error) {
    qDebug() << "error" << error << this->socket->errorString();
}


QUrl DriveSystemPrivate::createSocketUrl() const {
    auto url = this->baseUrl;
    url.setScheme("ws");
    url.setPath("/socket/v1/changes");
    return url;
}


QUrl DriveSystemPrivate::createApiUrl(const QString & path) const {
    auto url = this->baseUrl;
    url.setScheme("http");
    url.setPath(path);
    return url;
}


DriveNodeSP DriveSystemPrivate::createNode(const DriveFileInfo & fileInfo, DriveNodeSP parent) {
    DriveNodeSP node(new DriveNode(fileInfo, parent), [this](DriveNode * p) -> void {
        auto it = this->database.find(p->info.id());
        if (it != this->database.end()) {
            this->database.erase(it);
        }
        delete p;
    });
    return node;
}
