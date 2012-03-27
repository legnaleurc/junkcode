#include "napplication_p.hpp"

#include <algorithm>
#include <cassert>

std::shared_ptr< NApplication > NApplication::Private::self;

void NApplication::Private::destroy( NApplication * p ) {
	delete p;
}

NApplication::Private::Private():
focusWidget( NULL ),
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

void NApplication::Private::addWidget( NWidget * widget ) {
	// FIXME thread safety
	this->widgets.push_back( widget );
	if( this->widgets.size() == 1 ) {
		this->focusWidget = widget;
	}
}

void NApplication::Private::removeWidget( NWidget * widget ) {
	// FIXME thread safety
	this->widgets.erase( std::remove( std::begin( this->widgets ), std::end( this->widgets ), widget ), std::end( this->widgets ) );
	if( this->focusWidget == widget ) {
		this->focusWidget = NULL;
	}
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
		this->p_->focusWidget->inputEvent( c );
	}
	return 0;
}

void NApplication::quit() {
	std::exit( 0 );
}
