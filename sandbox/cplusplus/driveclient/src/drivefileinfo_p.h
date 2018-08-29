#ifndef DRIVEFILEINFO_H_
#define DRIVEFILEINFO_H_

#include "drivefileinfo.h"

#include <QtCore/QSharedData>
#include <QtCore/QVariantMap>


class DriveFileInfoPrivate : public QSharedData {
    Q_DECLARE_PUBLIC(DriveFileInfo)
public:
    DriveFileInfoPrivate(DriveFileInfo * q);
    DriveFileInfoPrivate(const QVariantMap & data);
    DriveFileInfoPrivate(DriveFileInfo * q, const QFileInfo & fileInfo);
    DriveFileInfoPrivate(DriveFileInfo * q, const QString & parentPath, const QString & name);

    void fetch();

    DriveFileInfo * q_ptr;
    QFileInfo fileInfo;
    bool fetched;
    bool isFolder;
};


#endif
