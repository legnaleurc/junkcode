#include "classumlitem.hpp"

#include <QtGui/QPainter>

namespace {

	const bool REGISTERD = AbstractUMLItem::signup( 0, []( QGraphicsItem * parent )->AbstractUMLItem * {
		return new ClassUMLItem( parent );
	} );

}

ClassUMLItem::ClassUMLItem( QGraphicsItem * parent ):
AbstractUMLItem( parent ) {
}

QRectF ClassUMLItem::boundingRect() const {
	return QRectF( -75.0, -75.0, 150.0, 150.0 );
}

void ClassUMLItem::paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget ) {
	painter->fillRect( this->boundingRect(), QColor( "yellow" ) );
	painter->setPen( this->pen() );
	painter->drawRect( this->boundingRect() );
	painter->drawLine( QPointF( -75.0, -25.0 ), QPointF( 75.0, -25.0 ) );
	painter->drawLine( QPointF( -75.0, 25.0 ), QPointF( 75.0, 25.0 ) );
	QFont font( painter->font() );
	font.setPointSize( 12 );
	painter->setFont( font );
	painter->drawText( QRectF( -75.0, -75.0, 150.0, 50.0 ), Qt::AlignCenter, this->getName() );
	this->AbstractUMLItem::paint( painter, option, widget );
}

int ClassUMLItem::type() const {
	return ClassUMLItem::Type;
}
