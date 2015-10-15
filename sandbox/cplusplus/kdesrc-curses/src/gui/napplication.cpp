#include "gui/napplication_p.hpp"

#include <algorithm>
#include <cassert>
#include <csignal>


std::shared_ptr<NApplication> NApplication::Private::self;

void NApplication::Private::destroy (NApplication * p) {
    delete p;
}

void NApplication::Private::onWindowChanged (int sig) {
    int rows = 0, cols = 0;
    getmaxyx(self->p_->screen.get(), rows, cols);
    self->p_->resize(rows, cols);
}

NApplication::Private::Private ():
focusWidget(nullptr),
screen(initscr(), [](WINDOW * w)->void {
    delwin(w);
    endwin();
}),
size(),
topLevelWidget(nullptr),
widgets() {
    if(!this->screen) {
        assert(!"screen initialization failed");
    }
    noecho();
    cbreak();
    keypad(this->screen.get(), TRUE);
    refresh();

    getmaxyx(this->screen.get(), this->size.rows(), this->size.cols());
}

NApplication::Private::~Private () {
    keypad(this->screen.get(), FALSE);
    nocbreak();
    echo();
}

// FIXME thread safety
void NApplication::Private::addWidget (NWidget * widget) {
    if (this->widgets.empty()) {
        this->focusWidget = widget;
        this->topLevelWidget = widget;
    }
    this->widgets.push_back(widget);
}

// FIXME thread safety
void NApplication::Private::removeWidget (NWidget * widget) {
    auto end = std::end(this->widgets);
    auto tail = std::remove(std::begin(this->widgets), end, widget);
    this->widgets.erase(tail, end);

    if( this->focusWidget == widget ) {
        this->focusWidget = nullptr;
    }
    if( this->topLevelWidget == widget ) {
        this->topLevelWidget = nullptr;
    }
}

void NApplication::Private::resize (int rows, int cols) {
    this->size.rows() = rows;
    this->size.cols() = cols;
    wresize(this->screen.get(), rows, cols);
    wrefresh(this->screen.get());
    if (this->topLevelWidget) {
        this->topLevelWidget->resize(rows, cols);
        this->topLevelWidget->update();
    }
}

NApplication & NApplication::instance () {
    if (!Private::self) {
        Private::self.reset(new NApplication, Private::destroy);
    }
    return *Private::self;
}

// FIXME maybe need to pass arguments
NApplication::NApplication ():
p_(new Private) {
}

NApplication::~NApplication () {
}

int NApplication::exec () {
    for (;;) {
        for (auto w : this->p_->widgets) {
            w->update();
        }
        // TODO dispatch events to focusing widget
        int c = getch();
        switch (c) {
        case KEY_RESIZE:
            Private::onWindowChanged(SIGWINCH);
            break;
        default:
            if (this->p_->focusWidget) {
                this->p_->focusWidget->inputEvent(c);
            }
        }
    }
    return 0;
}

const NSize & NApplication::size () const {
    return this->p_->size;
}

void NApplication::quit () {
    std::exit(0);
}
