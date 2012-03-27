#include "napplication_p.hpp"

#include <algorithm>
#include <cassert>
#include <csignal>

std::shared_ptr< NApplication > NApplication::Private::self;

void NApplication::Private::destroy( NApplication * p ) {
	delete p;
}

void NApplication::Private::onWindowChanged( int sig ) {
	int rows = 0, cols = 0;
	getmaxyx( self->p_->screen.get(), rows, cols );
	self->p_->resize( rows, cols );
}

NApplication::Private::Private():
focusWidget( nullptr ),
screen( initscr(), []( WINDOW * w ) {
	delwin( w );
	endwin();
} ),
size(),
widgets() {
	if( !this->screen ) {
		assert( !"" );
	}
	noecho();
	cbreak();
	keypad( this->screen.get(), TRUE );
	refresh();

	getmaxyx( this->screen.get(), this->size.rows(), this->size.cols() );
}

NApplication::Private::~Private() {
	keypad( this->screen.get(), FALSE );
	nocbreak();
	echo();
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

void NApplication::Private::resize( int rows, int cols ) {
	this->size.rows() = rows;
	this->size.cols() = cols;
	wresize( this->screen.get(), rows, cols );
	wrefresh( this->screen.get() );
	// FIXME top widget only
	std::for_each( std::begin( this->widgets ), std::end( this->widgets ), [rows, cols]( NWidget * w ) {
		w->resize( rows, cols );
		w->update();
	} );
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
	for(;;) {
		std::for_each( std::begin( this->p_->widgets ), std::end( this->p_->widgets ), []( NWidget * w ) {
			w->update();
		} );
		// TODO dispatch events to focusing widget
		int c = getch();
		switch( c ) {
		case KEY_RESIZE:
			Private::onWindowChanged( SIGWINCH );
			break;
		default:
			this->p_->focusWidget->inputEvent( c );
		}
	}
	return 0;
}

const NSize & NApplication::size() const {
	return this->p_->size;
}

void NApplication::quit() {
	std::exit( 0 );
}
