#ifndef QTCOROUTINE_HPP
#define QTCOROUTINE_HPP

#include <QtCore/QObject>

#include <functional>
#include <memory>

class QtCoroutine: public QObject {
    Q_OBJECT
public:
    class Yield;
    typedef std::function<void (const Yield &)> Callback;

    explicit QtCoroutine (Callback task, QObject *parent = 0);

    void start ();

signals:
    void finished ();

private:
    friend class Yield;
    class Private;
    class YieldPrivate;
    Private * d;
};

class QtCoroutine::Yield {
public:
    void operator () (int interval) const;
    void operator () (QObject * object, const char * signal_) const;

private:
    friend class QtCoroutine;

    Yield (std::shared_ptr<QtCoroutine::YieldPrivate> d);
    Yield (const Yield &);
    Yield & operator = (const Yield &);
    Yield (Yield &&);
    Yield & operator = (Yield &&);
    ~Yield();

    std::shared_ptr<QtCoroutine::YieldPrivate> d;
};

#endif
