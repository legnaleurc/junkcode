#include "umlitemgroup.hpp"
#include "abstractumlitem.hpp"
#include "abstractumllink.hpp"

#include <QtGui/QPainter>

#include <algorithm>

UMLItemGroup::UMLItemGroup( QGraphicsItem * parent ):
QGraphicsItemGroup( parent ) {
	this->setFlags( QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemSendsGeometryChanges );
}

void UMLItemGroup::paint( QPainter * painter, const QStyleOptionGraphicsItem * /*option*/, QWidget * /*widget*/ ) {
	if( !this->isSelected() ) {
		return;
	}
	QPen pen( painter->pen() );
	pen.setColor( "red" );
	pen.setStyle( Qt::DotLine );
	painter->setPen( pen );
	painter->drawRect( this->boundingRect() );
}
