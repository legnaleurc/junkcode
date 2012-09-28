#ifndef ABSTRACTUMLITEMPRIVATE_HPP
#define ABSTRACTUMLITEMPRIVATE_HPP

#include "abstractumlitem.hpp"
#include "port.hpp"
#include "umlitemgroup.hpp"

class AbstractUMLItem::Private {
public:
	explicit Private( AbstractUMLItem * host );

	std::shared_ptr< Port > topPort;
	std::shared_ptr< Port > bottomPort;
	std::shared_ptr< Port > leftPort;
	std::shared_ptr< Port > rightPort;
	QString name;
};

#endif
