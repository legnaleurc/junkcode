#include "keyboardhandler.hpp"

#include <unistd.h>
#include <fcntl.h>


using fbv::KeyboardHandler;


KeyboardHandler::KeyboardHandler(QObject * parent):
QObject(parent),
_notifier(new QSocketNotifier(STDIN_FILENO, QSocketNotifier::Read, this)),
_orig() {
    this->connect(this->_notifier, SIGNAL(activated(int)), SLOT(_onActivated(int)));

    tcgetattr(STDIN_FILENO, &this->_orig);
}

KeyboardHandler::~KeyboardHandler() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &this->_orig);
}

void KeyboardHandler::_onActivated(int skt) {
    ;
}
