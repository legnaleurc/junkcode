#ifndef DRIVEFILEINFO_H
#define DRIVEFILEINFO_H


#include <QtCore/QMetaType>
#include <QtCore/QFileInfo>
#include <QtCore/QDateTime>

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
    DriveFileInfo(const DriveSystem * driveSystem, const QString & path);
    DriveFileInfo(const DriveSystem * driveSystem, const QString & parentPath, const QString & name);

    bool operator == (const DriveFileInfo & that) const;

    QString fileName() const;
    bool isDir() const;
    bool exists() const;
    bool isSymLink() const;
    bool isFile() const;
    qint64 size() const;
    const QDateTime & lastModified() const;
    bool isHidden() const;
    QFile::Permissions permissions() const;
    QString mimeType() const;

    void setFile(const QString & path);

private:
    using Private = DriveFileInfoPrivate;

    std::shared_ptr<Private> d;
};

Q_DECLARE_METATYPE(DriveFileInfo)


#endif
