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
    DriveFileInfoPrivate(const DriveSystem * driveSystem, const QVariantMap & data);

    void fetch();

    std::mutex lock;
    const DriveSystem * driveSystem;
    bool fetched;
    QString id;
    QString fileName;
    bool isFolder;
    QDateTime mtime;
    qint64 size;
    bool exists;
    QString mimeType;
};


#endif
