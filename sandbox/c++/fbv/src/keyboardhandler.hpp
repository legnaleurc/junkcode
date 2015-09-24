#ifndef FBV_KEYBOARDHANDLER_HPP
#define FBV_KEYBOARDHANDLER_HPP


#include <QtCore/QObject>
#include <QtCore/QSocketNotifier>

#include <termios.h>

#include <map>


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
    void q();
    void ctrl_c();

private slots:
    void _onActivated(int skt);

private:
    void _dispatch(const QByteArray & keys);
    void _addMapping(const QByteArray & keys, const QByteArray & signal);
    void _setupMapping();

    QSocketNotifier * _notifier;
    std::map<QByteArray, QByteArray> _mapper;
    struct termios _orig;
};

}

#endif
