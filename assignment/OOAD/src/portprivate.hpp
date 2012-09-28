#ifndef PORT_PRIVATE_HPP
#define PORT_PRIVATE_HPP

#include "port.hpp"
#include "abstractumllink.hpp"
#include "abstractumlitem.hpp"

#include <QList>

class Port::Private {
public:
	explicit Private( AbstractUMLItem * item );

	AbstractUMLItem * item;
	QPointF position;
	QList< AbstractUMLLink * > links;
};

#endif
