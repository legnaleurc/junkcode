#include "jobqueue.h"

#include <QtCore/QDateTime>

JobQueue::JobQueue(QObject * parent): QObject(),
_nextHandle(0LL),
_queue(),
_currentCallback(nullptr) {
}

qint64 JobQueue::runAt(qint64 timestamp, QtCoroutine::Callback callback) {
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    qint64 diff = timestamp - now;
    if (diff <= 0LL) {
        return 0LL;
    }

    auto callbackHelper = new QtCoroutine(callback);
    callbackHelper->connect(callbackHelper, SIGNAL(finished()), SLOT(deleteLater()));

    auto timer = new QtCoroutine([this, diff, callbackHelper](const QtCoroutine::Yield & yield)->void {
        yield(diff);

        this->_enqueue(callbackHelper);
        this->_dequeue();
    });

    qint64 handle = ++this->_nextHandle;
//    this->_handles.insert(std::make_pair(handle, std::make_tuple(timer, callbackHelper)));

    timer->start();

    return handle;
}

void JobQueue::_enqueue(QtCoroutine * callback) {
    this->_queue.push_back(callback);
}

void JobQueue::_dequeue() {
    if (this->_queue.empty()) {
        return;
    }
    if (this->_currentCallback) {
        return;
    }

    this->_currentCallback = this->_queue.front();
    this->_queue.pop_front();

    auto runner = new QtCoroutine([this](const QtCoroutine::Yield & yield)->void {
        this->_currentCallback->start();
        yield(this->_currentCallback, SIGNAL(finished()));
        this->_currentCallback = nullptr;

        this->_dequeue();
    });
    runner->connect(runner, SIGNAL(finished()), SLOT(deleteLater()));
    runner->start();
}
