#include "filesystemmodel.hpp"

#include <QtCore/QFile>
#include <QtCore/QMimeData>
#include <QtCore/QThread>
#include <QtCore/QUrl>

#include <functional>


namespace qtfs {

class IOWorker;


class FileSystemModel::Private : public QObject {
    Q_OBJECT

public:
    explicit Private(FileSystemModel * parent);
    virtual ~Private();

    void queueTasks(Qt::DropAction action, const QString & to,
                    const QList<QUrl> & urls);

signals:
    void queueRequested(Qt::DropAction action, const QString & to,
                        const QList<QUrl> & urls);

public:
    FileSystemModel * _;
    QThread * ioThread;
};


class IOWorker : public QObject {
    Q_OBJECT

public:
    IOWorker();

public slots:
    void processQueue(Qt::DropAction action, const QString & to,
                      const QList<QUrl> & urls);
};

}


using qtfs::FileSystemModel;
using qtfs::IOWorker;


FileSystemModel::FileSystemModel(QObject * parent)
    : QFileSystemModel(parent)
    , _(new Private(this))
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
    switch (action) {
    case Qt::CopyAction:
    case Qt::LinkAction:
    case Qt::MoveAction:
        break;
    default:
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


FileSystemModel::Private::Private(FileSystemModel * parent)
    : QObject(parent)
    , _(parent)
    , ioThread(new QThread(this))
{
    auto worker = new IOWorker;
    worker->moveToThread(this->ioThread);
    worker->connect(this->ioThread, &QThread::finished, &QObject::deleteLater);
    worker->connect(this, &Private::queueRequested, &IOWorker::processQueue);
    this->ioThread->start();
}


FileSystemModel::Private::~Private() {
    this->ioThread->quit();
    this->ioThread->wait();
}


void FileSystemModel::Private::queueTasks(Qt::DropAction action,
                                          const QString & to,
                                          const QList<QUrl> & urls) {
    ;
}


IOWorker::IOWorker()
    : QObject(nullptr)
{}


#include "filesystemmodel.moc"
