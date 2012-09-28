#include "usecaseumlitem.hpp"

#include <QtGui/QPainter>

namespace {

	const bool REGISTERD = AbstractUMLItem::signup( 1, []( QGraphicsItem * parent )->AbstractUMLItem * {
		return new UseCaseUMLItem( parent );
	} );

}

UseCaseUMLItem::UseCaseUMLItem( QGraphicsItem * parent ):
AbstractUMLItem( parent ) {
}

QRectF UseCaseUMLItem::boundingRect() const {
	return QRectF( -50, -75, 150, 100 );
}

void UseCaseUMLItem::paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget ) {
	QPainterPath path;
	path.addEllipse( this->boundingRect() );
	painter->setPen( this->pen() );
	painter->fillPath( path, QColor( "yellow" ) );
	painter->drawEllipse( this->boundingRect() );
	QFont font( painter->font() );
	font.setPointSize( 12 );
	painter->setFont( font );
	painter->drawText( this->boundingRect(), Qt::AlignCenter, this->getName() );
	this->AbstractUMLItem::paint( painter, option, widget );
}

int UseCaseUMLItem::type() const {
	return UseCaseUMLItem::Type;
}
