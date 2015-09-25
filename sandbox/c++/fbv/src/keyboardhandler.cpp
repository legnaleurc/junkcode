#include "keyboardhandler.hpp"

#include <QtCore/QMetaObject>
#include <QtCore/QtDebug>

#include <unistd.h>
#include <fcntl.h>


using fbv::KeyboardHandler;


KeyboardHandler::KeyboardHandler(QObject * parent):
QObject(parent),
_notifier(new QSocketNotifier(STDIN_FILENO, QSocketNotifier::Read, this)),
_mapper(),
_orig_term(),
_orig_stdin() {
    this->_setupMapping();

    this->connect(this->_notifier, SIGNAL(activated(int)), SLOT(_onActivated(int)));

    if (isatty(STDIN_FILENO)) {
        struct termios new_term;

        tcgetattr(STDIN_FILENO, &this->_orig_term);

        new_term = this->_orig_term;
        new_term.c_lflag &= ~(ICANON | ECHO | ECHOCTL | ECHONL);
        new_term.c_cflag |= HUPCL;
        new_term.c_cc[VMIN] = 0;

        tcsetattr(STDIN_FILENO, TCSANOW, &new_term);
    } else {
        this->_orig_stdin = fcntl(STDIN_FILENO, F_GETFL);
        fcntl(STDIN_FILENO, F_SETFL, this->_orig_stdin | O_NONBLOCK);
    }
}

KeyboardHandler::~KeyboardHandler() {
    if (isatty(STDIN_FILENO)) {
        tcsetattr(STDIN_FILENO, TCSANOW, &this->_orig_term);
    } else {
        fcntl(STDIN_FILENO, F_SETFL, this->_orig_stdin);
    }
}

void KeyboardHandler::_onActivated(int) {
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
    emit signal();
}

void KeyboardHandler::_addMapping(const QByteArray & keys, KHMFvv mf) {
    this->_mapper.insert(std::make_pair(keys, std::bind(mf, this)));
}

void KeyboardHandler::_setupMapping() {
    this->_addMapping("\x1b[A", &KeyboardHandler::up);
    this->_addMapping("\x1b[B", &KeyboardHandler::down);
    this->_addMapping("\x1b[D", &KeyboardHandler::left);
    this->_addMapping("\x1b[C", &KeyboardHandler::right);
    this->_addMapping("\x1b[5~", &KeyboardHandler::pageUp);
    this->_addMapping("\x1b[6~", &KeyboardHandler::pageDown);
    this->_addMapping("q", &KeyboardHandler::q);
}
