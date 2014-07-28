#ifndef TASK_H
#define TASK_H

#include <QtCore/QObject>

#include <functional>

class Task : public QObject {
    Q_OBJECT
public:
    typedef std::function<void (int)> Yield;
    typedef std::function<void (const Yield &)> Callback;

    explicit Task (Callback task, QObject *parent = 0);

    void start ();

signals:
    void finished ();

private:
    class Private;
    Private * d;
};

#endif // TASK_H
