#ifndef GENERALIZATIONUMLLINK_HPP
#define GENERALIZATIONUMLLINK_HPP

#include "abstractumllink.hpp"

class GeneralizationUMLLink : public AbstractUMLLink {
public:
	enum {
		Type = AbstractUMLLink::Type + 3
	};

	GeneralizationUMLLink( Port * begin, Port * end, QGraphicsItem * parent = 0 );

	virtual QRectF boundingRect() const;
	virtual void paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * parent = 0 );
	virtual int type() const;
};

#endif
