#ifndef QTFS_FILESYSTEMMODEL_HPP
#define QTFS_FILESYSTEMMODEL_HPP


#include <QtWidgets/QFileSystemModel>


namespace qtfs {

class FileSystemModel : public QFileSystemModel {
public:
    explicit FileSystemModel(QObject * parent);

    virtual bool dropMimeData(const QMimeData * data, Qt::DropAction action,
                              int row, int column,
                              const QModelIndex & parent) override;
};

}


#endif
