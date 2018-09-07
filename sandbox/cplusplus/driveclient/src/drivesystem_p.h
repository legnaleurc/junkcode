#ifndef DRIVESYSTEM_H_
#define DRIVESYSTEM_H_

#include "drivesystem.h"

#include <QtWebSockets/QWebSocket>


class DriveSystemPrivate : public QObject {
    Q_OBJECT
public:
    explicit DriveSystemPrivate(DriveSystem * parent);

    QVariant get(const QString & path, const QList<QPair<QString, QString>> & params);

public slots:
    void onMessage(const QString &);

public:
    DriveSystem * q;
    QString baseUrl;
    QWebSocket * socket;
};


#endif
