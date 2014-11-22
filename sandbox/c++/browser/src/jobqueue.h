#ifndef JOBQUEUE_H
#define JOBQUEUE_H

#include <unordered_map>
#include <tuple>
#include <deque>

#include <QtCore/QObject>

#include "qtcoroutine.hpp"

class JobQueue: public QObject {
    Q_OBJECT
public:
    JobQueue(QObject * parent = 0);

    qint64 runAt(qint64 timestamp, QtCoroutine::Callback callback);

private slots:
    void _dequeue();

private:
    void _enqueue(QtCoroutine * callback);

    qint64 _nextHandle;
    typedef std::unordered_map<qint64, std::tuple<QtCoroutine *, QtCoroutine *>> HandleTable;
    HandleTable _handles;
    std::deque<QtCoroutine *> _queue;
    QtCoroutine * _currentCallback;
};

#endif
