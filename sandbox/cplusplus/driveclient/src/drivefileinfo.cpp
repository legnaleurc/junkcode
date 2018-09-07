#include "drivefileinfo_p.h"


DriveFileInfo::DriveFileInfo()
    : d(std::make_shared<Private>())
{}


DriveFileInfo::DriveFileInfo(const DriveFileInfo & that)
    : d(that.d)
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
    if (this != &that) {
        d = that.d;
    }
    return *this;
}


bool DriveFileInfo::operator == (const DriveFileInfo & that) const {
    return d->id == that.d->id;
}


bool DriveFileInfo::operator != (const DriveFileInfo & that) const {
    return !(*this == that);
}


const QString & DriveFileInfo::fileName() const {
    return d->fileName;
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


const QString & DriveFileInfo::mimeType() const {
    d->fetch();
    return d->mimeType;
}


const QIcon & DriveFileInfo::icon() const {
    return d->icon;
}


QDebug operator <<(QDebug debug, const DriveFileInfo & info) {
    QDebugStateSaver saver(debug);
    debug.nospace() << "DriveFileInfo(" << info.fileName() << ")";
    return debug;
}


DriveFileInfoPrivate::DriveFileInfoPrivate()
    : lock()
    , driveSystem(nullptr)
    , fetched(false)
    , id()
    , fileName()
    , isFolder(false)
    , mtime()
    , size(0)
    , exists(false)
    , mimeType()
    , icon()
{}


DriveFileInfoPrivate::DriveFileInfoPrivate(const DriveSystem * driveSystem,
                                           const QVariantMap & data)
    : lock()
    , driveSystem(driveSystem)
    , fetched(true)
    , id(data.value("id").toString())
    , fileName(data.value("name").toString())
    , isFolder(data.value("is_folder").toBool())
    , mtime(data.value("mtime").value<QDateTime>())
    , size(data.value("size").value<qint64>())
    , exists(true)
    , mimeType(data.value("mime_type").toString())
    , icon()
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
