#ifndef DRIVESYSTEM_H_
#define DRIVESYSTEM_H_

#include "drivesystem.h"

#include <QtWebSockets/QWebSocket>

#include <memory>


class Node {
public:
    DriveFileInfo info;
    std::weak_ptr<Node> parent;
    QHash<QString, std::shared_ptr<Node>> children;
};


class DriveSystemPrivate : public QObject {
    Q_OBJECT
public:
    explicit DriveSystemPrivate(DriveSystem * parent);

    QVariant get(const QString & path, const QList<QPair<QString, QString>> & params);

    void applyChange(const QVariantMap & change);

public slots:
    void onMessage(const QString & message);
    void onError(QAbstractSocket::SocketError error);

signals:
    void removed(const QString & id);
    void updated(const DriveFileInfo & info);

public:
    DriveSystem * q;
    QString baseUrl;
    QWebSocket * socket;
    std::shared_ptr<Node> root;
    QHash<QString, std::weak_ptr<Node>> database;
};


#endif
