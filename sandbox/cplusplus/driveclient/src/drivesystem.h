#ifndef DRIVESYSTEM_H
#define DRIVESYSTEM_H

#include "drivefileinfo.h"
#include "drivenode.h"

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QList>


class DriveSystemPrivate;


class DriveSystem : public QObject {
    Q_OBJECT
public:
    explicit DriveSystem(QObject * parent);

    DriveSystem(const DriveSystem &) = delete;
    DriveSystem & operator = (const DriveSystem &) = delete;

    DriveFileInfo setBaseUrl(const QString & host, int port);
    DriveNodeSP root() const;
    DriveNodeSP node(const QString & id) const;
    DriveNodeSP upsertNode(const DriveFileInfo & info);
    DriveNodeSP deleteNode(const QString & id);

    DriveFileInfo info(const QString & idOrPath) const;
    QList<DriveFileInfo> list(const QString & idOrPath) const;

signals:
    void directoryUpdated(const QString & id);

private:
    using Private = DriveSystemPrivate;

    Private * d;
};

#endif
