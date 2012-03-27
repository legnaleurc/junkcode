#include "nwidget_p.hpp"
#include "napplication_p.hpp"

#include <algorithm>

NWidget::Private::Private( NWidget * parent ):
children(),
parent( parent ),
pos(),
window( NULL ) {
	if( parent == NULL ) {
		this->window = newwin( 0, 0, 0, 0 );
	} else {
		NPoint pos = parent->pos();
		this->window = newwin( 0, 0, pos.y(), pos.x() );
	}
	box( this->window, 0, 0 );
}

NWidget::Private::~Private() {
	std::for_each( std::begin( this->children ), std::end( this->children ), []( NWidget * c ) {
		delete c;
	} );

	delwin( this->window );
}

NWidget::NWidget( NWidget * parent ):
p_( new Private( parent ) ) {
	if( parent != NULL ) {
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

void NWidget::update() {
	wrefresh( this->p_->window );
}
