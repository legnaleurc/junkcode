#ifndef TASK_P_H
#define TASK_P_H

#include "task.hpp"

#include <boost/coroutine/stack_allocator.hpp>
#include <boost/coroutine/coroutine.hpp>

class Task::Private: public QObject {
    Q_OBJECT
public:
    typedef boost::coroutines::coroutine<int> Coroutine;

    Private (Task::Callback task, QObject * parent);

    void chain ();

signals:
    void finished ();

public slots:
    void onTimeout ();

public:
    Task::Callback task;
    Coroutine::pull_type fork;
};

#endif // TASK_P_H
