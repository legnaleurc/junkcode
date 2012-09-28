#ifndef UMLITEMGROUP_HPP
#define UMLITEMGROUP_HPP

#include <QtGui/QGraphicsItemGroup>

class UMLItemGroup : public QGraphicsItemGroup {
public:
	UMLItemGroup( QGraphicsItem * parent = 0 );

	virtual void paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );
};

#endif
