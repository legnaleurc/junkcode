#include "drivefileinfo_p.h"

#include <QtCore/QDir>


DriveFileInfo::DriveFileInfo()
    : d_ptr(new DriveFileInfoPrivate(this))
{}


DriveFileInfo::DriveFileInfo(const QString & path)
    : d_ptr(new DriveFileInfoPrivate(this, path))
{
    Q_D(DriveFileInfo);
    d->fileInfo.setFile(path);
}


DriveFileInfo::DriveFileInfo(const QString & parentPath, const QString & name)
    : d_ptr(new DriveFileInfoPrivate(this, parentPath, name))
{
    Q_D(DriveFileInfo);
    QDir parent(parentPath);
    d->fileInfo = QFileInfo(parent, name);
}


DriveFileInfo::DriveFileInfo(const QFileInfo & fileInfo)
    : d_ptr(new DriveFileInfoPrivate(this, fileInfo))
{
    Q_D(DriveFileInfo);
    d->fileInfo = fileInfo;
}


DriveFileInfo & DriveFileInfo::operator = (const QFileInfo & fileInfo) {
    d_ptr->fileInfo = fileInfo;
    return *this;
}
