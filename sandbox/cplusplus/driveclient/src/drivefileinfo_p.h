#ifndef DRIVEFILEINFO_H_
#define DRIVEFILEINFO_H_

#include "drivefileinfo.h"

#include <QtCore/QSharedData>
#include <QtCore/QVariantMap>

#include <mutex>


class DriveFileInfoPrivate {
public:
    DriveFileInfoPrivate();
    DriveFileInfoPrivate(const QVariantMap & data);
    DriveFileInfoPrivate(const QFileInfo & fileInfo);
    DriveFileInfoPrivate(const QString & parentPath, const QString & name);

    void fetch();

    std::mutex lock;
    QFileInfo fileInfo;
    bool fetched;
    bool isFolder;
    QDateTime mtime;
    qint64 size;
    bool exists;
};


#endif
