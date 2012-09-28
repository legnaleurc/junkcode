#ifndef ABSTRACTUMLLINKPRIVATE_HPP
#define ABSTRACTUMLLINKPRIVATE_HPP

#include "abstractumllink.hpp"
#include "port.hpp"

class AbstractUMLLink::Private {
public:
	Private( Port * begin, Port * end );

	Port * begin;
	Port * end;
};

#endif
