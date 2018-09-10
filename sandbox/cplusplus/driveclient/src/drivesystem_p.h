#ifndef DRIVESYSTEM_H_
#define DRIVESYSTEM_H_

#include "drivesystem.h"

#include <QtWebSockets/QWebSocket>

#include <memory>


class DriveSystemPrivate : public QObject {
    Q_OBJECT
public:
    explicit DriveSystemPrivate(DriveSystem * parent);

    QVariant get(const QString & path, const QList<QPair<QString, QString>> & params);
    DriveFileInfo fetchInfo(const QString & idOrPath);
    void upsertNode(const DriveFileInfo & fileInfo);
    void applyChange(const QVariantMap & change);
    QUrl createSocketUrl() const;
    QUrl createApiUrl(const QString & path) const;

public slots:
    void onMessage(const QString & message);
    void onError(QAbstractSocket::SocketError error);

signals:
    void directoryUpdated(const QString & id);

public:
    DriveSystem * q;
    QUrl baseUrl;
    QWebSocket * socket;
    DriveNodeSP root;
    std::unordered_map<QString, DriveNodeWP> database;
};


#endif
