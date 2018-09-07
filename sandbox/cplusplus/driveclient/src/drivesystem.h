#ifndef DRIVESYSTEM_H
#define DRIVESYSTEM_H

#include "drivefileinfo.h"

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QList>


class DriveSystemPrivate;


class DriveSystem : public QObject {
public:
    explicit DriveSystem(QObject * parent);

    DriveSystem(const DriveSystem &) = delete;
    DriveSystem & operator = (const DriveSystem &) = delete;

    void setBaseUrl(const QString & baseUrl);

    DriveFileInfo info(const QString & idOrPath) const;
    QList<DriveFileInfo> list(const QString & idOrPath) const;

private:
    using Private = DriveSystemPrivate;

    Private * d;
};

#endif
