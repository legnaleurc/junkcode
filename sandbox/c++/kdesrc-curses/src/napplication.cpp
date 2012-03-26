#include "napplication_p.hpp"

#include <algorithm>
#include <cassert>

std::shared_ptr< NApplication > NApplication::Private::self;

void NApplication::Private::destroy( NApplication * p ) {
	delete p;
}

NApplication::Private::Private():
screen( initscr() ),
widgets() {
	if( this->screen == NULL ) {
		assert( !"" );
	}
	noecho();
	cbreak();
	keypad( this->screen, true );
}

NApplication::Private::~Private() {
	keypad( this->screen, false );
	nocbreak();
	echo();
	endwin();
}

NApplication & NApplication::instance() {
	if( !Private::self ) {
		Private::self.reset( new NApplication, Private::destroy );
	}
	return *Private::self;
}

// FIXME maybe need to pass arguments
NApplication::NApplication():
p_( new Private ) {
}

NApplication::~NApplication() {
}

int NApplication::exec() {
	refresh();
	for(;;) {
		std::for_each( std::begin( this->p_->widgets ), std::end( this->p_->widgets ), []( NWidget * w ) {
			w->update();
		} );
		// TODO dispatch events to focusing widget
		int c = getch();
		if( c == 'q' ) {
			break;
		}
	}
	return 0;
}
