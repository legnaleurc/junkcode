#ifndef ASSOCIATIONUMLLINK_HPP
#define ASSOCIATIONUMLLINK_HPP

#include "abstractumllink.hpp"

class AssociationUMLLink : public AbstractUMLLink {
public:
	enum {
		Type = AbstractUMLLink::Type + 1
	};

	AssociationUMLLink( Port * begin, Port * end, QGraphicsItem * parent = 0 );

	virtual int type() const;
};

#endif
