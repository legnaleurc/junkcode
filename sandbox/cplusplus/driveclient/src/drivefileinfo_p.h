#ifndef DRIVEFILEINFO_H_
#define DRIVEFILEINFO_H_

#include "drivefileinfo.h"

#include <QtCore/QSharedData>


class DriveFileInfoPrivate : public QSharedData {
    Q_DECLARE_PUBLIC(DriveFileInfo)
public:
    DriveFileInfoPrivate(DriveFileInfo * q);
    DriveFileInfoPrivate(DriveFileInfo * q, const QFileInfo & fileInfo);
    DriveFileInfoPrivate(DriveFileInfo * q, const QString & parentPath, const QString & name);

    DriveFileInfo * q_ptr;
    QFileInfo fileInfo;
};


#endif
