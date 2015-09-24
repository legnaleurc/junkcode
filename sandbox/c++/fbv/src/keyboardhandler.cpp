#include "keyboardhandler.hpp"

#include <QtCore/QMetaObject>
#include <QtCore/QtDebug>

#include <unistd.h>


using fbv::KeyboardHandler;


KeyboardHandler::KeyboardHandler(QObject * parent):
QObject(parent),
_notifier(new QSocketNotifier(STDIN_FILENO, QSocketNotifier::Read, this)),
_mapper(),
_orig() {
    this->_setupMapping();

    this->connect(this->_notifier, SIGNAL(activated(int)), SLOT(_onActivated(int)));

    tcgetattr(STDIN_FILENO, &this->_orig);
    struct termios now = this->_orig;
    cfmakeraw(&now);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &now);
}

KeyboardHandler::~KeyboardHandler() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &this->_orig);
}

void KeyboardHandler::_onActivated(int skt) {
    this->_notifier->setEnabled(false);
    char buffer[8];
    auto size = read(STDIN_FILENO, buffer, sizeof(buffer));
    QByteArray keys(buffer, size);
    this->_dispatch(keys);
    this->_notifier->setEnabled(true);
}

void KeyboardHandler::_dispatch(const QByteArray & keys) {
    auto it = this->_mapper.find(keys);
    if (it == this->_mapper.end()) {
        return;
    }
    auto signal = it->second;
    QMetaObject::invokeMethod(this, signal);
}

void KeyboardHandler::_addMapping(const QByteArray & keys, const QByteArray & signal) {
    this->_mapper.insert(std::make_pair(keys, signal));
}

void KeyboardHandler::_setupMapping() {
    this->_addMapping("\x1b[A", SIGNAL(up()));
    this->_addMapping("\x1b[B", SIGNAL(down()));
    this->_addMapping("\x1b[D", SIGNAL(left()));
    this->_addMapping("\x1b[C", SIGNAL(right()));
    this->_addMapping("q", SIGNAL(q()));
}
