#include "nsize_p.hpp"

NSize::Private::Private( int rows, int cols ):
rows( rows ),
cols( cols ) {
}

NSize::NSize():
p_( new Private( 0, 0 ) ) {
}

NSize::NSize( int rows, int cols ):
p_( new Private( rows, cols ) ) {
}

NSize::NSize( const NSize & that ):
p_( new Private( that.p_->rows, that.p_->cols ) ) {
}

NSize & NSize::operator =( const NSize & that ) {
	if( &that == this ) {
		return *this;
	}
	this->p_->cols = that.p_->cols;
	this->p_->rows = that.p_->rows;
	return *this;
}

int & NSize::cols() {
	return this->p_->cols;
}

int NSize::cols() const {
	return this->p_->cols;
}

int & NSize::rows() {
	return this->p_->rows;
}

int NSize::rows() const {
	return this->p_->rows;
}
