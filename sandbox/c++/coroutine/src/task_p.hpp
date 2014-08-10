#ifndef TASK_P_H
#define TASK_P_H

#include "task.hpp"

#include <boost/coroutine/stack_allocator.hpp>
#include <boost/coroutine/coroutine.hpp>

class Task::Private: public QObject {
    Q_OBJECT
public:
    typedef boost::coroutines::coroutine<void> Coroutine;

    Private (Task::Callback task, QObject * parent);

    void tail ();

signals:
    void finished ();

public slots:
    void postAction ();

public:
    Task::Callback task;
    Coroutine::pull_type fork;
};

class Task::YieldPrivate {
public:
    typedef Task::Private::Coroutine Coroutine;

    YieldPrivate (Task & task, Coroutine::push_type & yield);

    Task & task;
    Coroutine::push_type & yield;
};

class SignalIsolator : public QObject {
    Q_OBJECT
signals:
    void proxy();
};

#endif // TASK_P_H
