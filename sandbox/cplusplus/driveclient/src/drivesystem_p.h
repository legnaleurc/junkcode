#ifndef DRIVESYSTEM_H_
#define DRIVESYSTEM_H_

#include "drivesystem.h"

#include <QtWebSockets/QWebSocket>

#include <memory>
#include <unordered_map>


namespace std {

template<>
struct hash<QString> {
    using argument_type = QString;
    using result_type = uint;
    result_type operator ()(const argument_type & s) const noexcept {
        return qHash(s);
    }
};

}


class Node {
public:
    DriveFileInfo info;
    std::weak_ptr<Node> parent;
    std::unordered_map<QString, std::shared_ptr<Node>> children;
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
    std::unordered_map<QString, std::weak_ptr<Node>> database;
};


#endif
