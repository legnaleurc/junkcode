#include <climits>
#include <cmath>
#include <functional>
#include <iomanip>
#include <sstream>

#include "biginteger_p.hpp"
#include "valueerror.hpp"

#if defined( __GNUC__ ) && !defined(__clang__)
#	include <boost/regex.hpp>
#else
#	include <regex>
#endif

using bi::BigInteger;

BigInteger::Division::Private::Private( const BigInteger & q, const BigInteger & r ): q( q ), r( r ) {
}

BigInteger::Division::Division( const BigInteger & q, const BigInteger & r ): p_( new Private( q, r ) ) {
}

const BigInteger & BigInteger::Division::getQuotient() const {
	return this->p_->q;
}

const BigInteger & BigInteger::Division::getRemainder() const {
	return this->p_->r;
}

BigInteger::Private::Private(): v(), minus( false ) {
	this->v.push_back( 0 );
}

BigInteger::Private::Private( long l ): v(), minus( l < 0 ) {
	this->assign( l );
}

BigInteger::Private::Private( const std::string & s ): v(), minus( false ) {
	this->assign( s );
}

BigInteger::Private::Private( bool minus, const std::vector< int > & v ): v( v ), minus( minus ) {
}

void BigInteger::Private::assign( long l ) {
	if( l == 0L ) {
		this->v.push_back( 0 );
		return;
	}
	if( this->minus ) {
		l = -l;
	}
	while( l != 0L ) {
		int j = l % 1000L;
		this->v.push_back( j );
		l /= 1000L;
	}
}

void BigInteger::Private::assign( const std::string & s ) {
#if defined( __GNUC__ ) && !defined(__clang__)
	using boost::regex;
	using boost::smatch;
	using boost::regex_match;
#else
	using std::regex;
	using std::smatch;
	using std::regex_match;
#endif

	regex pattern( "^(0)|((-?)([1-9]\\d*))$" );
	smatch m;
	if( !regex_match( s, m, pattern ) ) {
		throw ValueError( "invalid number" );
	}
	if( m[1].matched ) {
		this->v.push_back( 0 );
	} else {
		if( m[3].length() != 0 ) {
			this->minus = true;
		}
		std::string buffer( m[4] );
		// add padding to 3 multiple
		if( s.size() % 3 != 0 ) {
			buffer.insert( 0, 3 - s.size() % 3, ' ' );
		}
		for( std::size_t i = 0; i < buffer.size(); i += 3 ) {
			std::istringstream sin( buffer.substr( i, 3 ) );
			int j = -1;
			sin >> j;
			this->v.insert( this->v.begin(), j );
		}
	}
}

void BigInteger::Private::normalize() {
	while( this->v.size() > 1 && this->v.back() == 0 ) {
		this->v.pop_back();
	}
	if( this->v.size() == 1 && this->v[0] == 0 ) {
		this->minus = false;
	}
}

BigInteger::BigInteger(): p_( new Private ) {
}

BigInteger::BigInteger( int i ): p_( new Private( i ) ) {
}

BigInteger::BigInteger( long l ): p_( new Private( l ) ) {
}

BigInteger::BigInteger( const char * c ): p_( new Private( std::string( c ) ) ) {
}

BigInteger::BigInteger( const char * c, std::size_t n ): p_( new Private( std::string( c, n ) ) ) {
}

BigInteger::BigInteger( const std::string & s ): p_( new Private( s ) ) {
}

BigInteger::BigInteger( const BigInteger & that ): p_( new Private( *that.p_ ) ) {
}

BigInteger::BigInteger( BigInteger && that ): p_( move( that.p_ ) ) {
}

BigInteger::BigInteger( std::unique_ptr< Private > && p ): p_( move( p ) ) {
}

BigInteger::~BigInteger() {
}

BigInteger & BigInteger::operator =( const BigInteger & that ) {
	if( &that == this ) {
		return *this;
	}

	this->p_->minus = that.p_->minus;
	this->p_->v = that.p_->v;

	return *this;
}

BigInteger & BigInteger::operator =( BigInteger && that ) {
	this->p_.swap( that.p_ );

	return *this;
}

BigInteger & BigInteger::operator +=( const BigInteger & that ) {
	if( this->p_->minus != that.p_->minus ) {
		if( !this->p_->minus ) {
			return *this -= -that;
		} else {
			return *this = that - -*this;
		}
	}

	int carry = 0;
	std::size_t minlen = std::min( this->p_->v.size(), that.p_->v.size() );
	std::size_t i = 0;
	for( i = 0; i < minlen; ++i ) {
		this->p_->v[i] += that.p_->v[i] + carry;
		carry = this->p_->v[i] / 1000;
		this->p_->v[i] %= 1000;
	}
	if( this->p_->v.size() > that.p_->v.size() ) {
		this->p_->v[i] += carry;
	} else if( this->p_->v.size() < that.p_->v.size() ) {
		auto it = that.p_->v.begin();
		std::advance( it, i );
		this->p_->v.insert( this->p_->v.end(), it, that.p_->v.end() );
		this->p_->v[i] += carry;
	} else if( carry != 0 ) {
		this->p_->v.push_back( carry );
	}

	return *this;
}

BigInteger & BigInteger::operator -=( const BigInteger & that ) {
	if( this->p_->minus != that.p_->minus ) {
		return *this += -that;
	}

	auto bigger = &this->p_->v;
	auto little = &that.p_->v;
	if( *this < that ) {
		this->p_->minus = !this->p_->minus;
		std::swap( bigger, little );
	}
	std::size_t i = 0;
	for( i = 0; i < little->size(); ++i ) {
		this->p_->v[i] = bigger->at( i ) - little->at( i );
		if( this->p_->v[i] < 0 ) {
			this->p_->v[i+1] -= 1;
			this->p_->v[i] += 1000;
		}
	}

	this->p_->normalize();
	return *this;
}

BigInteger & BigInteger::operator *=( const BigInteger & that ) {
	return *this = std::move( *this * that );
}

BigInteger & BigInteger::operator /=( const BigInteger & that ) {
	return *this = std::move( *this / that );
}

BigInteger & BigInteger::operator %=( const BigInteger & that ) {
	return *this = std::move( *this % that );
}

BigInteger::Division BigInteger::divide( const BigInteger & that ) const {
	return BigInteger::Division( that, that );
}

void BigInteger::swap( BigInteger & that ) {
	if( this != &that ) {
		this->p_.swap( that.p_ );
	}
}

namespace bi {

template<>
int BigInteger::to< int >() const {
	if( *this > INT_MAX ) {
		throw ValueError( "overflow INT_MAX" );
	}
	if( *this < INT_MIN ) {
		throw ValueError( "underflow INT_MIN" );
	}
	int tmp = 0;
	for( std::size_t i = 0; i < this->p_->v.size(); ++i ) {
		tmp += this->p_->v[i] * static_cast< int >( std::pow( 1000, static_cast< double >( i ) ) );
	}
	return tmp;
}

template<>
long BigInteger::to< long >() const {
	if( *this > LONG_MAX ) {
		throw ValueError( "overflow LONG_MAX" );
	}
	if( *this < LONG_MIN ) {
		throw ValueError( "underflow LONG_MIN" );
	}
	long tmp = 0;
	for( std::size_t i = 0; i < this->p_->v.size(); ++i ) {
		tmp += this->p_->v[i] * static_cast< long >( std::pow( 1000, static_cast< double >( i ) ) );
	}
	return tmp;
}

template<>
std::string BigInteger::to< std::string >() const {
	std::ostringstream sout;
	sout << *this;
	return move( sout.str() );
}

}

BigInteger bi::operator +( const BigInteger & a ) {
	BigInteger b( a );
	return std::move( +std::move( b ) );
}

BigInteger bi::operator +( BigInteger && a ) {
	return std::move( a );
}

BigInteger bi::operator -( const BigInteger & a ) {
	BigInteger b( a );
	return std::move( -std::move( b ) );
}

BigInteger bi::operator -( BigInteger && a ) {
	if( !( a.p_->v.size() == 1 && a.p_->v[0] == 0 ) ) {
		a.p_->minus = !a.p_->minus;
	}
	return std::move( a );
}

bool bi::operator <( const BigInteger & a, const BigInteger & b ) {
	if( a.p_->minus && !b.p_->minus ) {
		return true;
	} else if( !a.p_->minus && b.p_->minus ) {
		return false;
	}
	auto little = &a.p_->v;
	auto bigger = &b.p_->v;
	if( a.p_->minus ) {
		swap( little, bigger );
	}
	if( little->size() != bigger->size() ) {
		return little->size() < bigger->size();
	}
	for( std::size_t i = little->size(); i-- != 0; ) {
		if( little->at( i ) < bigger->at( i ) ) {
			return true;
		}
	}
	return false;
}

bool bi::operator <=( const BigInteger & a, const BigInteger & b ) {
	return !( a > b );
}

bool bi::operator ==( const BigInteger & a, const BigInteger & b ) {
	return a.p_->minus == b.p_->minus && a.p_->v == b.p_->v;
}

bool bi::operator >=( const BigInteger & a, const BigInteger & b ) {
	return !( a < b );
}

bool bi::operator >( const BigInteger & a, const BigInteger & b ) {
	return b < a;
}

bool bi::operator !=( const BigInteger & a, const BigInteger & b ) {
	return !( a == b );
}

BigInteger bi::operator +( const BigInteger & a, const BigInteger & b ) {
	return std::move( BigInteger( a ) += b );
}

BigInteger bi::operator -( const BigInteger & a, const BigInteger & b ) {
	return std::move( BigInteger( a ) -= b );
}

BigInteger bi::operator *( const BigInteger & a, const BigInteger & b ) {
	std::vector< int > result( a.p_->v.size() + b.p_->v.size(), 0 );

	std::size_t base = 0U;
	for( auto ait = begin( a.p_->v ); ait != end( a.p_->v ); ++ait ) {
		int carry = 0;
		std::size_t offset = 0U;
		for( auto bit = begin( b.p_->v ); bit != end( b.p_->v ); ++bit ) {
			result[base + offset] += *ait * *bit + carry;
			carry = result[base + offset] / 1000;
			result[base + offset] %= 1000;

			++offset;
		}
		result[base + offset] += carry;
		++base;
	}

	std::unique_ptr< BigInteger::Private > p( new BigInteger::Private( a.p_->minus != b.p_->minus, result ) );
	p->normalize();

	return std::move( BigInteger( move( p ) ) );
}

BigInteger bi::operator /( const BigInteger & a, const BigInteger & b ) {
	return a.divide( b ).getQuotient();
}

BigInteger bi::operator %( const BigInteger & a, const BigInteger & b ) {
	return a.divide( b ).getRemainder();
}

BigInteger & bi::operator ++( BigInteger & a ) {
	return a += 1;
}

BigInteger bi::operator ++( BigInteger & a, int ) {
	BigInteger b( a );
	++a;
	return b;
}

BigInteger & bi::operator --( BigInteger & a ) {
	return a -= 1;
}

BigInteger bi::operator --( BigInteger & a, int ) {
	BigInteger b( a );
	--a;
	return b;
}

std::istream & bi::operator >>( std::istream & in, BigInteger & self ) {
	std::string s;
	in >> s;
	self = s;
	return in;
}

std::ostream & bi::operator <<( std::ostream & out, const BigInteger & self ) {
	if( self.p_->minus ) {
		out << '-';
	}
	auto it = self.p_->v.rbegin();
	out << *it;
	for( advance( it, 1 ); it != self.p_->v.rend(); ++it ) {
		out << std::setfill( '0' ) << std::setw( 3 ) << *it;
	}
	return out;
}

void bi::swap( BigInteger & a, BigInteger & b ) {
	a.swap( b );
}

namespace std {
template<>
void swap( bi::BigInteger & a, bi::BigInteger & b ) {
	a.swap( b );
}
}
