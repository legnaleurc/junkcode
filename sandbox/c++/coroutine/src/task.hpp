#ifndef TASK_H
#define TASK_H

#include <QtCore/QObject>

#include <functional>
#include <memory>

class Task: public QObject {
    Q_OBJECT
public:
    class Yield;
    typedef std::function<void (const Yield &)> Callback;

    explicit Task (Callback task, QObject *parent = 0);

    void start ();

    bool isFinished () const;

signals:
    void finished ();

private:
    friend class Yield;
    class Private;
    class YieldPrivate;
    Private * d;
};

class Task::Yield {
public:
    void operator () (int interval) const;
    void operator () (QObject * object, const char * signal_) const;

private:
    friend class Task;
    Yield (std::shared_ptr<Task::YieldPrivate> d);
    std::shared_ptr<Task::YieldPrivate> d;
};

#endif // TASK_H
