#ifndef BI_BIGINTEGER_P_HPP
#define BI_BIGINTEGER_P_HPP

#include <vector>

#include "biginteger.hpp"

namespace bi {

class BigInteger::Private {
public:
	Private();
	explicit Private( long );
	explicit Private( const std::string & );
	Private( bool minus, const std::vector< int > & v );

	void assign( long );
	void assign( const std::string & );
	void normalize();

	std::vector< int > v;
	bool minus;
};

}

#endif
