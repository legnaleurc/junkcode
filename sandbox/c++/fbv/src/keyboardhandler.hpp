#ifndef FBV_KEYBOARDHANDLER_HPP
#define FBV_KEYBOARDHANDLER_HPP


#include <QtCore/QObject>
#include <QtCore/QSocketNotifier>

#include <termios.h>


namespace fbv {

class KeyboardHandler: public QObject {
    Q_OBJECT

public:
    KeyboardHandler(QObject * parent = 0);
    ~KeyboardHandler();

private slots:
    void _onActivated(int skt);

private:
    QSocketNotifier * _notifier;
    struct termios _orig;
};

}

#endif
