#ifndef TASK_H
#define TASK_H

#include <functional>

#include <QtCore/QObject>
#include <QtCore/QVariant>

class Task : public QObject {
    Q_OBJECT
public:
    typedef std::function<void (QVariant)> Yield;
    typedef std::function<void (const Yield &)> Callback;

    explicit Task(Callback task, QObject *parent = 0);

    void start();

signals:
    void finished();

public slots:
    void notify(const QString & response);

private:
    class Private;
    Private * d;
};

#endif // TASK_H
