#include "drivefileinfo_p.h"


DriveFileInfo::DriveFileInfo()
    : d(std::make_shared<Private>())
{}


DriveFileInfo::DriveFileInfo(const DriveFileInfo & that)
    : d(std::make_shared<Private>(that.d->fileInfo))
{}


DriveFileInfo::DriveFileInfo(const QString & path)
    : d(std::make_shared<Private>(path))
{}


DriveFileInfo::DriveFileInfo(const QString & parentPath, const QString & name)
    : d(std::make_shared<Private>(parentPath, name))
{}


DriveFileInfo::DriveFileInfo(const QFileInfo & fileInfo)
    : d(std::make_shared<Private>(fileInfo))
{}

DriveFileInfo::~DriveFileInfo() noexcept
{}


DriveFileInfo & DriveFileInfo::operator = (const DriveFileInfo & that) {
    return *this;
}


DriveFileInfo & DriveFileInfo::operator = (const QFileInfo & fileInfo) {
    if (d->fileInfo != fileInfo) {
        d = std::make_shared<Private>(fileInfo);
    }
    return *this;
}


QString DriveFileInfo::fileName() const {
    return d->fileInfo.fileName();
}


bool DriveFileInfo::isDir() const {
    if (!d->fetched) {
        d->fetch();
    }
    return d->isFolder;
}


void DriveFileInfo::setFile(const QString & path) {
    d->fileInfo.setFile(path);
    d->fetched = false;
}
