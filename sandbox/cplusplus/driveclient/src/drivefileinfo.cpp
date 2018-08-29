#include "drivefileinfo_p.h"


DriveFileInfo::DriveFileInfo()
    : d_ptr(new DriveFileInfoPrivate(this))
{}


DriveFileInfo::DriveFileInfo(const DriveFileInfo & that)
    : d_ptr(new DriveFileInfoPrivate(this, that.d_ptr->fileInfo))
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

DriveFileInfo::~DriveFileInfo()
{}


DriveFileInfo & DriveFileInfo::operator = (const DriveFileInfo & that) {
    return *this;
}


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
        assert(!"not fetched");
    }
    return d->isFolder;
}


void DriveFileInfo::setFile(const QString & path) {
    d_ptr->fileInfo.setFile(path);
    d_ptr->fetched = false;
}
