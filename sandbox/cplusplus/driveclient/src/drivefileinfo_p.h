#ifndef DRIVEFILEINFO_H_
#define DRIVEFILEINFO_H_

#include "drivefileinfo.h"
#include "drivesystem.h"

#include <QtCore/QSharedData>
#include <QtCore/QVariantMap>

#include <mutex>


class DriveFileInfoPrivate {
public:
    DriveFileInfoPrivate();
    DriveFileInfoPrivate(const DriveSystem * driveSystem, const QString & parentPath, const QVariantMap & data);
    DriveFileInfoPrivate(const DriveSystem * driveSystem, const QFileInfo & fileInfo);
    DriveFileInfoPrivate(const DriveSystem * driveSystem, const QString & parentPath, const QString & name);

    void fetch();

    std::mutex lock;
    const DriveSystem * driveSystem;
    QFileInfo fileInfo;
    bool fetched;
    QString id;
    bool isFolder;
    QDateTime mtime;
    qint64 size;
    bool exists;
};


#endif
