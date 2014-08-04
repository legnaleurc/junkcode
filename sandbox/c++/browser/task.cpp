#include "task.h"
#include "task_p.h"

#include <cassert>

#include <QtCore/QTimer>

Task::Task(Callback task, QObject *parent) :
    QObject(parent),
    d(new Private(task, this))
{
    this->connect(this->d, SIGNAL(finished()), SIGNAL(finished()));
}

void Task::start() {
    this->d->fork = Private::Coroutine::pull_type([&](Private::Coroutine::push_type & yield)->void {
        this->d->task([&](QVariant condition)->void {
            yield(condition);
        });
    });
    this->d->chain();
}

void Task::notify(const QString &response) {
    if (this->d->response != response) {
        return;
    }
    // consume this condition
    this->d->response.clear();
    this->d->onTimeout();
}

Task::Private::Private(Callback task, QObject *parent) :
    QObject(parent),
    task(task),
    fork(),
    response()
{
}

void Task::Private::chain() {
    if (!this->fork) {
        emit this->finished();
        return;
    }
    auto condition = this->fork.get();
    if (condition.type() == QVariant::Int) {
        int interval = condition.toInt();
        QTimer::singleShot(interval, this, SLOT(onTimeout()));
    } else if (condition.type() == QVariant::String) {
        this->response = condition.toString();
    }
}

void Task::Private::onTimeout() {
    assert(this->fork || !"coroutine error");
    this->fork();
    this->chain();
}
