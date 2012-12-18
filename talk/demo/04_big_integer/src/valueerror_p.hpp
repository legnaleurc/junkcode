#ifndef BI_VALUEERROR_P_HPP
#define BI_VALUEERROR_P_HPP

#include "valueerror.hpp"

namespace bi {

class ValueError::Private {
public:
	Private( const std::string & );

	std::string msg;
};

}

#endif
