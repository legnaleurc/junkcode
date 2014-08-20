#ifndef QTCOROUTINE_HPP
#define QTCOROUTINE_HPP

#include <QtCore/QObject>

#include <functional>
#include <memory>

#ifdef QTCOROUTINE_LIBRARY
#define QTCOROUTINE_DLL Q_DECL_EXPORT
#else
#define QTCOROUTINE_DLL Q_DECL_IMPORT
#endif

class QTCOROUTINE_DLL QtCoroutine : public QObject {
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

class QTCOROUTINE_DLL QtCoroutine::Yield {
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
