#ifndef COMPOSITIONUMLLINK_HPP
#define COMPOSITIONUMLLINK_HPP

#include "abstractumllink.hpp"

class CompositionUMLLink : public AbstractUMLLink {
public:
	enum {
		Type = AbstractUMLLink::Type + 2
	};

	CompositionUMLLink( Port * begin, Port * end, QGraphicsItem * parent = 0 );

	virtual QRectF boundingRect() const;
	virtual void paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * parent = 0 );
	virtual int type() const;
};

#endif
