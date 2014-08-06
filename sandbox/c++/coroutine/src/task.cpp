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
            yield(interval);
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
	return;
    }
    int interval = this->fork.get();
    QTimer::singleShot(interval, this, SLOT(onTimeout()));
}

void Task::Private::onTimeout () {
    assert(this->fork || !"invalid coroutine");
    this->fork();
    this->chain();
}
