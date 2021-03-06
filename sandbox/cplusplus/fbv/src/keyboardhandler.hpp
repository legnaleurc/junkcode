#ifndef FBV_KEYBOARDHANDLER_HPP
#define FBV_KEYBOARDHANDLER_HPP


#include <QtCore/QObject>
#include <QtCore/QSocketNotifier>

#include <termios.h>

#include <map>
#include <functional>


namespace fbv {

class KeyboardHandler: public QObject {
    Q_OBJECT

public:
    KeyboardHandler(QObject * parent = 0);
    virtual ~KeyboardHandler();

signals:
    void up();
    void down();
    void left();
    void right();
    void pageUp();
    void pageDown();
    void q();
    void ctrlC();

private slots:
    void _onActivated(int skt);

private:
    typedef void (KeyboardHandler::*KHMFvv)();
    typedef std::function<void ()> CB;

    void _dispatch(const QByteArray & keys);
    void _addMapping(const QByteArray & keys, KHMFvv mf);
    void _setupMapping();

    QSocketNotifier * _notifier;
    std::map<QByteArray, CB> _mapper;
    struct termios _orig_term;
    int _orig_stdin;
};

}

#endif
