#include "drivefileinfo_p.h"

#include <QtCore/QDir>


DriveFileInfo::DriveFileInfo()
    : d(std::make_shared<Private>())
{}


DriveFileInfo::DriveFileInfo(const DriveFileInfo & that)
    : d(that.d)
{
    assert(d->driveSystem);
}


DriveFileInfo::DriveFileInfo(const DriveSystem * driveSystem, const QString & path)
    : d(std::make_shared<Private>(driveSystem, path))
{
    assert(d->driveSystem);
}


DriveFileInfo::DriveFileInfo(const DriveSystem * driveSystem, const QString & parentPath, const QString & name)
    : d(std::make_shared<Private>(driveSystem, parentPath, name))
{
    assert(d->driveSystem);
}

DriveFileInfo::DriveFileInfo(DriveFileInfoPrivate * d)
    : d(d)
{
    assert(d->driveSystem);
    assert(d->fetched);
}

DriveFileInfo::~DriveFileInfo() noexcept
{}


DriveFileInfo & DriveFileInfo::operator = (const DriveFileInfo & that) {
    d = that.d;
    return *this;
}


bool DriveFileInfo::operator == (const DriveFileInfo & that) const {
    return d->fileInfo == that.d->fileInfo;
}


QString DriveFileInfo::fileName() const {
    return d->fileInfo.fileName();
}


bool DriveFileInfo::isDir() const {
    d->fetch();
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
    d->fetch();
    return d->mtime;
}


qint64 DriveFileInfo::size() const {
    d->fetch();
    return d->size;
}


bool DriveFileInfo::exists() const {
    d->fetch();
    return d->exists;
}


bool DriveFileInfo::isFile() const {
    return !this->isDir();
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


DriveFileInfoPrivate::DriveFileInfoPrivate()
    : lock()
    , driveSystem(nullptr)
    , fileInfo()
    , fetched(false)
    , id()
    , isFolder(false)
    , mtime()
    , size(0)
    , exists(false)
{}


DriveFileInfoPrivate::DriveFileInfoPrivate(const DriveSystem * driveSystem,
                                           const QString & parentPath,
                                           const QVariantMap & data)
    : lock()
    , driveSystem(driveSystem)
    , fileInfo(QDir(parentPath), data.value("name").value<QString>())
    , fetched(true)
    , id(data.value("id").value<QString>())
    , isFolder(data.value("is_folder").value<bool>())
    , mtime(data.value("mtime").value<QDateTime>())
    , size(data.value("size").value<qint64>())
    , exists(true)
{}


DriveFileInfoPrivate::DriveFileInfoPrivate(const DriveSystem * driveSystem,
                                           const QFileInfo & fileInfo)
    : lock()
    , driveSystem(driveSystem)
    , fileInfo(fileInfo)
    , fetched(false)
    , id()
    , isFolder(false)
    , mtime()
    , size(0)
    , exists(false)
{}


DriveFileInfoPrivate::DriveFileInfoPrivate(const DriveSystem * driveSystem,
                                           const QString & parentPath,
                                           const QString & fileName)
    : lock()
    , driveSystem(driveSystem)
    , fileInfo(QDir(parentPath), fileName)
    , fetched(false)
    , id()
    , isFolder(false)
    , mtime()
    , size(0)
    , exists(false)
{}


void DriveFileInfoPrivate::fetch() {
    std::unique_lock<std::mutex> locker(this->lock);
    if (this->fetched) {
        return;
    }
    auto info = this->driveSystem->info(this->id);
    this->isFolder = info.isDir();
    this->mtime = info.lastModified();
    this->size = info.size();
    this->exists = info.exists();
    this->fetched = true;
}
