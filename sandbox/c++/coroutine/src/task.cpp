#include "task.hpp"
#include "task_p.hpp"

#include <QtCore/QTimer>

#include <cassert>

Task::Task (Callback task, QObject *parent): QObject(parent),
d(new Private(task, this)) {
    this->connect(this->d, SIGNAL(finished()), SIGNAL(finished()));
}

void Task::start () {
    this->d->fork = Private::Coroutine::pull_type([&](Private::Coroutine::push_type & yield)->void {
        Yield yield_(std::make_shared<YieldPrivate>(*this, yield));
        this->d->task(yield_);
    });
    this->d->tail();
}

bool Task::isFinished () const {
    return !this->d->fork;
}

Task::Yield::Yield (std::shared_ptr<Task::YieldPrivate> d):
d(d) {
}

void Task::Yield::operator () (int interval) const {
    QTimer::singleShot(interval, this->d->task.d, SLOT(postAction()));
    this->d->yield();
}

void Task::Yield::operator () (QObject * object, const char * signal_) const {
    SignalIsolator si;
    si.connect(object, signal_, SIGNAL(proxy()));
    this->d->task.d->connect(&si, SIGNAL(proxy()), SLOT(postAction()));
    this->d->yield();
}

Task::Private::Private (Callback task, QObject *parent): QObject(parent),
task(task),
fork() {
}

void Task::Private::tail () {
    if (!this->fork) {
        emit this->finished();
    }
}

void Task::Private::postAction () {
    assert(this->fork || !"coroutine error");
    this->fork();
    this->tail();
}

Task::YieldPrivate::YieldPrivate (Task & task, Coroutine::push_type & yield):
task(task),
yield(yield) {
}
