#include "qtcoroutine.hpp"
#include "qtcoroutine_p.hpp"

#include <QtCore/QTimer>

#include <cassert>

QtCoroutine::QtCoroutine (Callback task, QObject *parent): QObject(parent),
d(new Private(task, this)) {
    this->connect(this->d, SIGNAL(finished()), SIGNAL(finished()));
}

void QtCoroutine::start () {
    this->d->fork = Private::Coroutine::pull_type([&](Private::Coroutine::push_type & yield)->void {
        Yield yield_(std::make_shared<YieldPrivate>(*this, yield));
        this->d->task(yield_);
    });
    this->d->tail();
}

QtCoroutine::Yield::Yield (std::shared_ptr<QtCoroutine::YieldPrivate> d):
d(d) {
}

void QtCoroutine::Yield::operator () (int interval) const {
    QTimer::singleShot(interval, this->d->task.d, SLOT(postAction()));
    this->d->yield();
}

void QtCoroutine::Yield::operator () (QObject * object, const char * signal_) const {
    SignalIsolator si;
    si.connect(object, signal_, SIGNAL(proxy()));
    this->d->task.d->connect(&si, SIGNAL(proxy()), SLOT(postAction()));
    this->d->yield();
}

QtCoroutine::Private::Private (Callback task, QObject *parent): QObject(parent),
task(task),
fork() {
}

void QtCoroutine::Private::tail () {
    if (!this->fork) {
        emit this->finished();
    }
}

void QtCoroutine::Private::postAction () {
    assert(this->fork || !"coroutine error");
    this->fork();
    this->tail();
}

QtCoroutine::YieldPrivate::YieldPrivate (QtCoroutine & task, Coroutine::push_type & yield):
task(task),
yield(yield) {
}
