#include "filesystemmodel.hpp"

#include <QtCore/QFile>
#include <QtCore/QMimeData>
#include <QtCore/QThread>
#include <QtCore/QUrl>

#include <functional>
#include <cassert>


namespace qtfs {

class IOWorker;

namespace detail {

class FileProgressDataPrivate {
public:
    bool finished;
    uint64_t current;
    uint64_t total;
    QString filePath;
    bool succeed;
};

} // namespace detail


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
    void progressUpdated(const FileProgressData & data);

public slots:
    void onTaskProcessed(const QString & filePath, bool succeed);

public:
    FileSystemModel * _;
    QThread * ioThread;
    uint64_t totalTasks;
    uint64_t finishedTasks;
};


class IOWorker : public QObject {
    Q_OBJECT

public:
    IOWorker();

signals:
    void processed(const QString & filePath, bool succeed);

public slots:
    void processQueue(Qt::DropAction action, const QString & to,
                      const QList<QUrl> & urls);
};


FileProgressData::FileProgressData(detail::FPDHandle data)
    : _(data)
{
}


bool FileProgressData::finished() const {
    return _->finished;
}


uint64_t FileProgressData::current() const {
    return _->current;
}


uint64_t FileProgressData::total() const {
    return _->total;
}


const QString & FileProgressData::filePath() const {
    return _->filePath;
}


bool FileProgressData::succeed() const {
    return _->succeed;
}

} // namespace qtfs


Q_DECLARE_METATYPE(Qt::DropAction);
Q_DECLARE_METATYPE(QList<QUrl>);


using qtfs::FileSystemModel;
using qtfs::IOWorker;


FileSystemModel::FileSystemModel(QObject * parent)
    : QFileSystemModel(parent)
    , _(new Private(this))
{
    QObject::connect(_, &Private::progressUpdated,
                     this, &FileSystemModel::progressUpdated);

    qRegisterMetaType<Qt::DropAction>();
    qRegisterMetaType<QList<QUrl>>();
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

    auto to = this->filePath(parent) + QDir::separator();
    _->queueTasks(action, to, data->urls());

    return true;
}


FileSystemModel::Private::Private(FileSystemModel * parent)
    : QObject(parent)
    , _(parent)
    , ioThread(new QThread(this))
    , totalTasks(0)
    , finishedTasks(0)
{
    auto worker = new IOWorker;
    worker->moveToThread(this->ioThread);
    QObject::connect(this->ioThread, &QThread::finished, worker, &QObject::deleteLater);
    QObject::connect(this, &Private::queueRequested, worker, &IOWorker::processQueue);
    QObject::connect(worker, &IOWorker::processed, this, &Private::onTaskProcessed);
    this->ioThread->start();
}


FileSystemModel::Private::~Private() {
    this->ioThread->quit();
    this->ioThread->wait();
}


void FileSystemModel::Private::queueTasks(Qt::DropAction action,
                                          const QString & to,
                                          const QList<QUrl> & urls) {
    this->totalTasks += urls.size();
    emit this->queueRequested(action, to, urls);
}


void FileSystemModel::Private::onTaskProcessed(const QString & filePath,
                                               bool succeed) {
    auto handle = std::make_shared<detail::FileProgressDataPrivate>();
    handle->finished = false;
    handle->current = this->finishedTasks;
    handle->total = this->totalTasks;
    handle->filePath = filePath;
    handle->succeed = succeed;
    FileProgressData data{std::move(handle)};
    emit this->progressUpdated(data);
}


IOWorker::IOWorker()
    : QObject(nullptr)
{}


void IOWorker::processQueue(Qt::DropAction action, const QString & to,
                            const QList<QUrl> & urls) {
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
        assert(!"reached dead code");
        return;
    }

    for (const auto & url : urls) {
        auto path = url.toLocalFile();
        bool ok = fn(path, to + QFileInfo(path).fileName());
        emit this->processed(path, ok);
    }
}


#include "filesystemmodel.moc"
