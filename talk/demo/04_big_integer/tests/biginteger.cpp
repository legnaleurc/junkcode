#include "biginteger.hpp"
#define BOOST_TEST_MODULE BigIntegerTest
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( default_constructor ) {
	using bi::BigInteger;
	BigInteger a;
	BOOST_CHECK_EQUAL( a, 0 );
	BOOST_CHECK_EQUAL( a, 0L );
	BOOST_CHECK_EQUAL( a, "0" );
	BOOST_CHECK_EQUAL( a, std::string( "0" ) );
}

BOOST_AUTO_TEST_CASE( constructor_int ) {
	using bi::BigInteger;
	BigInteger a = 9527;
	BOOST_CHECK_EQUAL( a, 9527 );
	BOOST_CHECK_EQUAL( a, 9527L );
	BOOST_CHECK_EQUAL( a, "9527" );
	BOOST_CHECK_EQUAL( a, std::string( "9527" ) );
}

BOOST_AUTO_TEST_CASE( constructor_long ) {
	using bi::BigInteger;
	BigInteger a = 9527L;
	BOOST_CHECK_EQUAL( a, 9527 );
	BOOST_CHECK_EQUAL( a, 9527L );
	BOOST_CHECK_EQUAL( a, "9527" );
	BOOST_CHECK_EQUAL( a, std::string( "9527" ) );
}

BOOST_AUTO_TEST_CASE( constructor_c_string ) {
	using bi::BigInteger;
	BigInteger a = "1234567890";
	BOOST_CHECK_EQUAL( a, "1234567890" );
	BOOST_CHECK_EQUAL( a, std::string( "1234567890" ) );
}

BOOST_AUTO_TEST_CASE( constructor_string ) {
	using bi::BigInteger;
	BigInteger a = std::string( "1234567890" );
	BOOST_CHECK_EQUAL( a, "1234567890" );
	BOOST_CHECK_EQUAL( a, std::string( "1234567890" ) );
}

BOOST_AUTO_TEST_CASE( copy_constructor ) {
	using bi::BigInteger;
	BigInteger a = "1234567890";
	BigInteger b = a;
	BOOST_CHECK_EQUAL( b, "1234567890" );
	BOOST_CHECK_EQUAL( b, std::string( "1234567890" ) );
}

BOOST_AUTO_TEST_CASE( move_constructor ) {
	using bi::BigInteger;
	BigInteger a = BigInteger( "1234567890" );
	BOOST_CHECK_EQUAL( a, "1234567890" );
	BOOST_CHECK_EQUAL( a, std::string( "1234567890" ) );
}

BOOST_AUTO_TEST_CASE( assignment_operator ) {
	using bi::BigInteger;
	BigInteger a = 1234;
	BigInteger b;
	b = a;
	BOOST_CHECK_EQUAL( b, 1234 );
}

BOOST_AUTO_TEST_CASE( move_assignment_operator ) {
	using bi::BigInteger;
	BigInteger b;
	b = 1234;
	BOOST_CHECK_EQUAL( b, 1234 );
}

BOOST_AUTO_TEST_CASE( less_than ) {
	using bi::BigInteger;
	BigInteger a = 1234, b = 1678;
	BOOST_CHECK_LT( a, b );
}

BOOST_AUTO_TEST_CASE( less_than_or_equal ) {
	using bi::BigInteger;
	BigInteger a = 1234, b = 5678, c = 5678;
	BOOST_CHECK_LE( a, b );
	BOOST_CHECK_LE( b, c );
}

BOOST_AUTO_TEST_CASE( equal ) {
	using bi::BigInteger;
	BigInteger a = 1234, b = 1234;
	BOOST_CHECK_EQUAL( a, b );
}

BOOST_AUTO_TEST_CASE( greater_than_or_equal ) {
	using bi::BigInteger;
	BigInteger a = 1234, b = 5678, c = 1234;
	BOOST_CHECK_GE( a, c );
	BOOST_CHECK_GE( b, a );
}

BOOST_AUTO_TEST_CASE( greater_than ) {
	using bi::BigInteger;
	BigInteger a = 1234, b = 1678;
	BOOST_CHECK_GT( b, a );
}

BOOST_AUTO_TEST_CASE( not_equal ) {
	using bi::BigInteger;
	BigInteger a = 1234, b = 5678;
	BOOST_CHECK_NE( b, a );
}

BOOST_AUTO_TEST_CASE( negative ) {
	using bi::BigInteger;
	BigInteger a = 1234;
	a = -a;
	BOOST_CHECK_EQUAL( a, -1234 );
}

BOOST_AUTO_TEST_CASE( plus ) {
	using bi::BigInteger;
	BigInteger a = 1234;
	a = a + a;
	BOOST_CHECK_EQUAL( a, 2468 );
}

BOOST_AUTO_TEST_CASE( plus_assignment ) {
	using bi::BigInteger;
	BigInteger a = 1234;
	a += a;
	BOOST_CHECK_EQUAL( a, 2468 );
}

BOOST_AUTO_TEST_CASE( subtract ) {
	using bi::BigInteger;
	BigInteger a = 1234, b = 2468;
	a = a - b;
	BOOST_CHECK_EQUAL( a, -1234 );
}

BOOST_AUTO_TEST_CASE( subtract_assignment ) {
	using bi::BigInteger;
	BigInteger a = 1234, b = 2468;
	a -= b;
	BOOST_CHECK_EQUAL( a, -1234 );
}

BOOST_AUTO_TEST_CASE( convert_to_int ) {
	using bi::BigInteger;
	BigInteger a = 1234;
	BOOST_CHECK_EQUAL( a.to< int >(), 1234 );
}

BOOST_AUTO_TEST_CASE( convert_to_long ) {
	using bi::BigInteger;
	BigInteger a = 1234L;
	BOOST_CHECK_EQUAL( a.to< long >(), 1234L );
}

BOOST_AUTO_TEST_CASE( convert_to_string ) {
	using bi::BigInteger;
	BigInteger a = "1234567890";
	BOOST_CHECK_EQUAL( a.to< std::string >(), "1234567890" );
}

BOOST_AUTO_TEST_CASE( member_swap ) {
	using bi::BigInteger;
	BigInteger a = 1234, b = 5678;
	a.swap( b );
	BOOST_CHECK_EQUAL( a, 5678 );
	BOOST_CHECK_EQUAL( b, 1234 );
}

BOOST_AUTO_TEST_CASE( free_swap ) {
	using bi::BigInteger;
	BigInteger a = 1234, b = 5678;
	swap( a, b );
	BOOST_CHECK_EQUAL( a, 5678 );
	BOOST_CHECK_EQUAL( b, 1234 );
}

BOOST_AUTO_TEST_CASE( std_swap ) {
	using bi::BigInteger;
	BigInteger a = 1234, b = 5678;
	std::swap( a, b );
	BOOST_CHECK_EQUAL( a, 5678 );
	BOOST_CHECK_EQUAL( b, 1234 );
}
