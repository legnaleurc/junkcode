#ifndef NPOINT_P_HPP
#define NPOINT_P_HPP

#include "core/npoint.hpp"

class NPoint::Private {
public:
	Private( int x, int y );

	int x;
	int y;
};

#endif
