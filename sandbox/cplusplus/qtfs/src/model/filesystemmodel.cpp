#include "filesystemmodel.hpp"

#include <QtCore/QFile>
#include <QtCore/QMimeData>
#include <QtCore/QUrl>

#include <functional>


using qtfs::FileSystemModel;


FileSystemModel::FileSystemModel(QObject * parent)
    : QFileSystemModel(parent)
{
}


bool FileSystemModel::dropMimeData(const QMimeData * data,
                                   Qt::DropAction action, int /*row*/,
                                   int /*column*/, const QModelIndex & parent)
{
    if (!parent.isValid()) {
        return false;
    }
    if (this->isReadOnly()) {
        return false;
    }

    using FnType = bool (const QString &, const QString &);
    std::function<FnType> fn;

    switch (action) {
    case Qt::CopyAction:
        fn = static_cast<FnType *>(QFile::copy);
        break;
    case Qt::LinkAction:
        fn = static_cast<FnType *>(QFile::link);
        break;
    case Qt::MoveAction:
        fn = static_cast<FnType *>(QFile::rename);
        break;
    default:
        return false;
    }

    bool ok = false;
    auto to = this->filePath(parent) + QDir::separator();

    auto urls = data->urls();

    for (const auto & url : urls) {
        auto path = url.toLocalFile();
        ok = fn(path, to + QFileInfo(path).fileName()) && ok;
    }

    return ok;
}
