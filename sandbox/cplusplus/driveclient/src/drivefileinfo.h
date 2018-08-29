#ifndef DRIVEFILEINFO_H
#define DRIVEFILEINFO_H


#include <QtCore/QMetaType>
#include <QtCore/QFileInfo>
#include <QtCore/QDateTime>


class DriveFileInfo {
public:
    DriveFileInfo();
    explicit DriveFileInfo(const QString & path);
    DriveFileInfo(const QString & parentPath, const QString name);
    explicit DriveFileInfo(const QFileInfo & fileInfo);

    ~DriveFileInfo();

    DriveFileInfo & operator = (const QFileInfo & fileInfo);

    bool operator == (const DriveFileInfo & that) const;

    QString fileName() const;
    bool isDir() const;
    bool exists() const;
    bool isSymLink() const;
    bool isFile() const;
    const QFileInfo & fileInfo() const;
    qint64 size() const;
    const QDateTime & lastModified() const;
    bool isHidden() const;
    QFile::Permissions permissions() const;

    void setFile(const QString & path);
};


Q_DECLARE_METATYPE(DriveFileInfo)


#endif
