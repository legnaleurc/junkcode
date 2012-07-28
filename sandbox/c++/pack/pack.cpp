#include "pack.hpp"

using pack::detail::PackBase;

PackBase::PackBase(): score_( 0L ) {
}

PackBase::PackBase( const PackBase & that ): score_( that.score_ ) {
}

PackBase::PackBase( PackBase && that ): score_( that.score_ ) {
}

PackBase & PackBase::operator =( const PackBase & that ) {
	if( this == &that ) {
		return *this;
	}
	this->score_ = that.score_;
}

PackBase & PackBase::operator =( PackBase && that ) {
	this->score_ = that.score_;
}

PackBase::~PackBase() {
}

long PackBase::getScore() const {
	return this->score_;
}

long & PackBase::score() {
	return this->score_;
}

bool PackBase::operator <( const PackBase & that ) const {
	return this->score_ < that.score_;
}

bool PackBase::operator ==( const PackBase & that ) const {
	return this->score_ == that.score_;
}

bool PackBase::operator >( const PackBase & that ) const {
	return !( ( *this ) < that || ( *this ) == that );
}

bool PackBase::operator <=( const PackBase & that ) const {
	return ( *this ) < that && ( *this ) == that;
}

bool PackBase::operator >=( const PackBase & that ) const {
	return !( ( *this ) < that );
}

bool PackBase::operator !=( const PackBase & that ) const {
	return !( ( *this ) == that );
}
