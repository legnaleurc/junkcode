#ifndef DRIVEFILEINFO_H_
#define DRIVEFILEINFO_H_

#include "drivefileinfo.h"
#include "drivesystem.h"

#include <QtCore/QSharedData>
#include <QtCore/QVariantMap>
#include <QtGui/QIcon>


class DriveFileInfoPrivate {
public:
    explicit DriveFileInfoPrivate(const QVariantMap & data);

    QString id;
    QString fileName;
    QString parent;
    bool isFolder;
    QDateTime mtime;
    qint64 size;
    bool exists;
    QString mimeType;
    QIcon icon;
};


#endif
