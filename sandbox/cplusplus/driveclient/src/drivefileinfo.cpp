#include "drivefileinfo_p.h"


DriveFileInfo::DriveFileInfo()
    : d()
{}


DriveFileInfo::DriveFileInfo(const DriveFileInfo & that)
    : d(that.d)
{
}


DriveFileInfo::DriveFileInfo(DriveFileInfoPrivate * d)
    : d(d)
{
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


bool DriveFileInfo::isValid() const {
    return !!d;
}


const QString & DriveFileInfo::id() const {
    return d->id;
}


const QString & DriveFileInfo::parentId() const {
    return d->parentId;
}


const QString & DriveFileInfo::fileName() const {
    return d->fileName;
}


bool DriveFileInfo::isDir() const {
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
    return d->mtime;
}


qint64 DriveFileInfo::size() const {
    return d->size;
}


bool DriveFileInfo::exists() const {
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


DriveFileInfoPrivate::DriveFileInfoPrivate(const QVariantMap & data)
    : id(data.value("id").toString())
    , fileName(data.value("name").toString())
    , parentId(data.value("parent_id").toString())
    , isFolder(data.value("is_folder").toBool())
    , mtime(data.value("modified").toDateTime())
    , size(data.value("size").value<qint64>())
    , exists(!data.value("trashed").toBool())
    , mimeType(data.value("mime_type").toString())
    , icon()
{}
