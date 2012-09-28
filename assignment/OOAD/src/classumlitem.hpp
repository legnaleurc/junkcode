#ifndef CLASSUMLITEM_HPP
#define CLASSUMLITEM_HPP

#include "abstractumlitem.hpp"

class ClassUMLItem : public AbstractUMLItem {
public:
	enum {
		Type = AbstractUMLItem::Type + 1
	};

	ClassUMLItem( QGraphicsItem * parent = 0 );

	virtual QRectF boundingRect() const;
	virtual void paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );
	virtual int type() const;
};

#endif
