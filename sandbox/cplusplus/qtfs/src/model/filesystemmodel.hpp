#ifndef QTFS_FILESYSTEMMODEL_HPP
#define QTFS_FILESYSTEMMODEL_HPP


#include <QtWidgets/QFileSystemModel>

#include <memory>


namespace qtfs {

class FileProgressData;
namespace detail {
class FileProgressDataPrivate;
using FPDHandle = std::shared_ptr<FileProgressDataPrivate>;
}


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
    explicit FileProgressData(detail::FPDHandle);

    bool finished() const;
    uint64_t current() const;
    uint64_t total() const;
    const QString & filePath() const;
    bool succeed() const;

private:
    detail::FPDHandle _;
};

}


#endif
