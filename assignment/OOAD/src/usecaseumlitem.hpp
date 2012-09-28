#ifndef USECASEUMLITEM_HPP
#define USECASEUMLITEM_HPP

#include "abstractumlitem.hpp"

class UseCaseUMLItem : public AbstractUMLItem {
public:
	enum {
		Type = AbstractUMLItem::Type + 2
	};

	UseCaseUMLItem( QGraphicsItem * parent = 0 );

	virtual QRectF boundingRect() const;
	virtual void paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );
	virtual int type() const;
};

#endif
