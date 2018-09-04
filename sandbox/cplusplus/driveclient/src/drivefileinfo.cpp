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

DriveFileInfo::DriveFileInfo(DriveFileInfoPrivate * d)
    : d(d)
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


bool DriveFileInfo::operator == (const DriveFileInfo & that) const {
    return d->fileInfo == that.d->fileInfo;
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


QFile::Permissions DriveFileInfo::permissions() const {
    auto flags = QFile::ReadUser | QFile::WriteUser;
    if (this->isDir()) {
        flags |= QFile::ExeUser;
    }
    return flags;
}


const QDateTime & DriveFileInfo::lastModified() const {
    if (!d->fetched) {
        d->fetch();
    }
    return d->mtime;
}


qint64 DriveFileInfo::size() const {
    if (!d->fetched) {
        d->fetch();
    }
    return d->size;
}


bool DriveFileInfo::exists() const {
    if (!d->fetched) {
        d->fetch();
    }
    return d->exists;
}


bool DriveFileInfo::isFile() const {
    return !this->isDir();
}


const QFileInfo & DriveFileInfo::fileInfo() const {
    return d->fileInfo;
}


bool DriveFileInfo::isHidden() const {
    return false;
}


bool DriveFileInfo::isSymLink() const {
    return false;
}


void DriveFileInfo::setFile(const QString & path) {
    d->fileInfo.setFile(path);
    d->fetched = false;
}
