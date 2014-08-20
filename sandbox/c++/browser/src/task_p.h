#ifndef TASK_P_H
#define TASK_P_H

#include "task.h"

#include <boost/coroutine/stack_allocator.hpp>
#include <boost/coroutine/coroutine.hpp>

class Task::Private: public QObject {
    Q_OBJECT
public:
    typedef boost::coroutines::coroutine<QVariant> Coroutine;

    Private(Task::Callback task, QObject * parent);

    void chain();

signals:
    void finished();

public slots:
    void onTimeout();

public:
    Task::Callback task;
    Coroutine::pull_type fork;
    QString response;
};

#endif // TASK_P_H
