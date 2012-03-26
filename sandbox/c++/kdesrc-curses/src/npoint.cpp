#include "npoint_p.hpp"

NPoint::Private::Private( int x, int y ):
x( x ),
y( y ) {
}

NPoint::NPoint():
p_( new Private( 0, 0 ) ) {
}

NPoint::NPoint( int x, int y ):
p_( new Private( x, y ) ) {
}

NPoint::NPoint( const NPoint & that ):
p_( new Private( that.x(), that.y() ) ) {
}

NPoint & NPoint::operator =( const NPoint & that ) {
	if( &that == this ) {
		return *this;
	}
	this->p_->x = that.x();
	this->p_->y = that.y();
	return *this;
}

int & NPoint::x() {
	return this->p_->x;
}

int NPoint::x() const {
	return this->p_->x;
}

int & NPoint::y() {
	return this->p_->y;
}

int NPoint::y() const {
	return this->p_->y;
}
