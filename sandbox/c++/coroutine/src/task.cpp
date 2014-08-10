#include "task.hpp"
#include "task_p.hpp"

#include <QtCore/QTimer>

#include <cassert>

Task::Task (Callback task, QObject * parent): QObject(parent),
d(new Private(task, this)) {
    this->connect(this->d, SIGNAL(finished()), SIGNAL(finished()));
}

void Task::start () {
    this->d->fork = Private::Coroutine::pull_type([&](Private::Coroutine::push_type & yield)->void {
        this->d->task([&](int interval)->void {
            QTimer::singleShot(interval, this->d, SLOT(onTimeout()));
            yield();
        });
    });
    this->d->chain();
}

Task::Private::Private (Callback task, QObject * parent): QObject(parent),
task(task),
fork() {
}

void Task::Private::chain () {
    if (!this->fork) {
        emit this->finished();
    }
}

void Task::Private::onTimeout () {
    assert(this->fork || !"invalid coroutine");
    this->fork();
    this->chain();
}
