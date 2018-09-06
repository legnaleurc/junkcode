#ifndef DRIVESYSTEM_H
#define DRIVESYSTEM_H

#include "drivefileinfo.h"

#include <QtCore/QString>
#include <QtCore/QList>

#include <memory>


class DriveSystemPrivate;


class DriveSystem {
public:
    DriveSystem();

    DriveSystem(const DriveSystem &) = delete;
    DriveSystem & operator = (const DriveSystem &) = delete;

    void setBaseUrl(const QString & baseUrl);

    DriveFileInfo info(const QString & idOrPath) const;
    QList<DriveFileInfo> list(const QString & idOrPath) const;

private:
    using Private = DriveSystemPrivate;
    using PrivateDeleter = void (*)(Private *);

    std::unique_ptr<Private, PrivateDeleter> d;
};

#endif
