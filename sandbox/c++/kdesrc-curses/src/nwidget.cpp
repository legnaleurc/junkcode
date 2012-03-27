#include "nwidget_p.hpp"
#include "napplication_p.hpp"

#include <algorithm>

namespace {
	void destory( WINDOW * w ) {
		delwin( w );
	}
}

NWidget::Private::Private( NWidget * parent ):
children(),
keyPressed(),
parent( parent ),
pos(),
size(),
window() {
	if( !parent ) {
		// no parent, screen-wide
		this->window.reset( newwin( 0, 0, 0, 0 ), destory );
		this->size = NApplication::instance().size();
	} else {
		NPoint pos = parent->pos();
		pos.y() += 1;
		pos.x() += 1;
		NSize size = parent->size();
		size.rows() -= 2;
		size.cols() -= 2;
		this->window.reset( derwin( parent->p_->window.get(), size.rows(), size.cols(), pos.y(), pos.x() ), destory );
		this->pos = pos;
		this->size = size;
	}
	box( this->window.get(), 0, 0 );
}

NWidget::Private::~Private() {
	std::for_each( std::begin( this->children ), std::end( this->children ), []( NWidget * c ) {
		delete c;
	} );
}

NWidget::NWidget( NWidget * parent ):
p_( new Private( parent ) ) {
	if( parent ) {
		parent->p_->children.push_back( this );
	}
	NApplication::instance().p_->addWidget( this );
}

NWidget::~NWidget() {
	NApplication::instance().p_->removeWidget( this );
}

NWidget * NWidget::parent() const {
	return this->p_->parent;
}

const NPoint & NWidget::pos() const {
	return this->p_->pos;
}

const NSize & NWidget::size() const {
	return this->p_->size;
}

void NWidget::resize( int rows, int cols ) {
	this->p_->size.rows() = rows;
	this->p_->size.cols() = cols;
	wresize( this->p_->window.get(), rows, cols );
	werase( this->p_->window.get() );
	box( this->p_->window.get(), 0, 0 );

	// update children
	std::for_each( std::begin( this->p_->children ), std::end( this->p_->children ), [rows, cols]( NWidget * w ) {
		w->resize( rows - 2, cols - 2 );
	} );
}

void NWidget::update() {
	wrefresh( this->p_->window.get() );
}

boost::signal< void ( int ) > & NWidget::keyPressed() {
	return this->p_->keyPressed;
}

void NWidget::inputEvent( int keyCode ) {
	this->p_->keyPressed( keyCode );
}
