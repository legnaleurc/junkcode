#ifndef DRIVEFILEINFO_H
#define DRIVEFILEINFO_H


#include <QtCore/QMetaType>
#include <QtCore/QFileInfo>
#include <QtCore/QDateTime>
#include <QtCore/QDebug>

#include <memory>


class DriveFileInfoPrivate;
class DriveSystem;


class DriveFileInfo {
public:
    DriveFileInfo();
    DriveFileInfo(const DriveFileInfo &);
    ~DriveFileInfo() noexcept;
    DriveFileInfo & operator = (const DriveFileInfo &);
    void swap(DriveFileInfo &) noexcept;

    explicit DriveFileInfo(DriveFileInfoPrivate *);

    bool operator == (const DriveFileInfo & that) const;
    bool operator != (const DriveFileInfo & that) const;

    const QString & fileName() const;
    bool isDir() const;
    bool exists() const;
    bool isSymLink() const;
    bool isFile() const;
    qint64 size() const;
    const QDateTime & lastModified() const;
    bool isHidden() const;
    QFile::Permissions permissions() const;
    const QString & mimeType() const;
    const QIcon & icon() const;

private:
    using Private = DriveFileInfoPrivate;

    std::shared_ptr<Private> d;
};


QDebug operator <<(QDebug debug, const DriveFileInfo & info);


Q_DECLARE_METATYPE(DriveFileInfo)


#endif
