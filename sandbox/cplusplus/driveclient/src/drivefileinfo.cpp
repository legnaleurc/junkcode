#include "drivefileinfo_p.h"


DriveFileInfo::DriveFileInfo()
    : d_ptr(new DriveFileInfoPrivate(this))
{}


DriveFileInfo::DriveFileInfo(const QString & path)
    : d_ptr(new DriveFileInfoPrivate(this, path))
{}


DriveFileInfo::DriveFileInfo(const QString & parentPath, const QString & name)
    : d_ptr(new DriveFileInfoPrivate(this, parentPath, name))
{}


DriveFileInfo::DriveFileInfo(const QFileInfo & fileInfo)
    : d_ptr(new DriveFileInfoPrivate(this, fileInfo))
{}


DriveFileInfo & DriveFileInfo::operator = (const QFileInfo & fileInfo) {
    d_ptr->fileInfo = fileInfo;
    return *this;
}


QString DriveFileInfo::fileName() const {
    Q_D(const DriveFileInfo);
    return d->fileInfo.fileName();
}


bool DriveFileInfo::isDir() const {
    Q_D(const DriveFileInfo);
    if (!d->fetched) {
        d->fetch();
    }
    return d->isFolder;
}
