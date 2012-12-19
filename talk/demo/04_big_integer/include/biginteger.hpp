#ifndef BI_BIGINTEGER_HPP
#define BI_BIGINTEGER_HPP

#include <memory>
#include <string>

namespace bi {

class BigInteger {
public:
	class Division {
	public:
		const BigInteger & getQuotient() const;
		const BigInteger & getRemainder() const;

	private:
		friend class BigInteger;
		Division( const BigInteger & q, const BigInteger & r );
		class Private;
		std::shared_ptr< Private > p_;
	};

	BigInteger();
	BigInteger( const BigInteger & that );
	BigInteger( BigInteger && that );
	BigInteger( int i );
	BigInteger( long l );
	BigInteger( const char * c );
	BigInteger( const char * c, std::size_t n );
	BigInteger( const std::string & s );
	~BigInteger();

	BigInteger & operator =( const BigInteger & that );
	BigInteger & operator =( BigInteger && that );

	BigInteger & operator +=( const BigInteger & that );
	BigInteger & operator -=( const BigInteger & that );
	BigInteger & operator *=( const BigInteger & that );
	BigInteger & operator /=( const BigInteger & that );
	BigInteger & operator %=( const BigInteger & that );

	Division divide( const BigInteger & that ) const;

	void swap( BigInteger & that );

	template< typename T >
	T to() const;

private:
	friend BigInteger operator -( BigInteger && a );
	friend bool operator <( const BigInteger & a, const BigInteger & b );
	friend bool operator ==( const BigInteger & a, const BigInteger & b );
	friend BigInteger operator *( const BigInteger & a, const BigInteger & b );
	friend std::istream & operator >>( std::istream & in, BigInteger & self );
	friend std::ostream & operator <<( std::ostream & out, const BigInteger & self );
	class Private;
	explicit BigInteger( std::unique_ptr< Private > && p );
	std::unique_ptr< Private > p_;
};

BigInteger operator +( const BigInteger & a );
BigInteger operator +( BigInteger && a );
BigInteger operator -( const BigInteger & a );
BigInteger operator -( BigInteger && a );

bool operator <( const BigInteger & a, const BigInteger & b );
bool operator <=( const BigInteger & a, const BigInteger & b );
bool operator ==( const BigInteger & a, const BigInteger & b );
bool operator >=( const BigInteger & a, const BigInteger & b );
bool operator >( const BigInteger & a, const BigInteger & b );
bool operator !=( const BigInteger & a, const BigInteger & b );

BigInteger operator +( const BigInteger & a, const BigInteger & b );
BigInteger operator -( const BigInteger & a, const BigInteger & b );
BigInteger operator *( const BigInteger & a, const BigInteger & b );
BigInteger operator /( const BigInteger & a, const BigInteger & b );
BigInteger operator %( const BigInteger & a, const BigInteger & b );

BigInteger & operator ++( BigInteger & a );
BigInteger operator ++( BigInteger & a, int );
BigInteger & operator --( BigInteger & a );
BigInteger operator --( BigInteger & a, int );

std::istream & operator >>( std::istream & in, BigInteger & self );
std::ostream & operator <<( std::ostream & out, const BigInteger & self );

void swap( BigInteger & a, BigInteger & b );

}

namespace std {

template<>
void swap( bi::BigInteger & a, bi::BigInteger & b );

}

#endif
