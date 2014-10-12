#ifndef JOBQUEUE_HPP
#define JOBQUEUE_HPP

#include <queue>

#include <QtCore/QObject>

#include "qtcoroutine.hpp"

class JobQueue: public QObject {
    Q_OBJECT
public:
    JobQueue ();

    QtCoroutine * setTimeout (int msIntevel, QtCoroutine * callback);

private slots:
    void _dequeue ();
    void _reset ();

private:
    void _enqueue (QtCoroutine * callback);

    std::queue<QtCoroutine *> _queue;
    QtCoroutine * _currentCallback;
};

#endif
