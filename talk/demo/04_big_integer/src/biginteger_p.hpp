#ifndef BI_BIGINTEGER_P_HPP
#define BI_BIGINTEGER_P_HPP

#include "biginteger.hpp"

#include <vector>

namespace bi {

	class BigInteger::Private {
	public:
		Private();
		explicit Private( long );
		explicit Private( const std::string & );

		void assign( long );
		void assign( const std::string & );
		void normalize();

		std::vector< int > v;
		bool minus;
	};

}

#endif
