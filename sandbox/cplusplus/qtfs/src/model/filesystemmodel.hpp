#ifndef QTFS_FILESYSTEMMODEL_HPP
#define QTFS_FILESYSTEMMODEL_HPP


#include <QtWidgets/QFileSystemModel>


namespace qtfs {

class FileProgressData;


class FileSystemModel : public QFileSystemModel {
    Q_OBJECT

public:
    explicit FileSystemModel(QObject * parent);

    virtual bool dropMimeData(const QMimeData * data, Qt::DropAction action,
                              int row, int column,
                              const QModelIndex & parent) override;

signals:
    void progressUpdated(const FileProgressData & data);

private:
    class Private;
    Private * _;
};


class FileProgressData final {
public:
    bool finished() const;
    uint64_t current() const;
    uint64_t total() const;
    const QString & filePath() const;
    bool succeed() const;
};

}


#endif
