#include "compositionumllink.hpp"

#include <QtGui/QPainter>

namespace {

	const bool REGISTERED = AbstractUMLLink::signup( 2, []( Port * begin, Port * end )->AbstractUMLLink * {
		return new CompositionUMLLink( begin, end );
	} );

	const qreal ARRAW_HEAD_SIZE = 8.0;

}

CompositionUMLLink::CompositionUMLLink( Port * begin, Port * end, QGraphicsItem * parent ):
AbstractUMLLink( begin, end, parent ) {
}

QRectF CompositionUMLLink::boundingRect() const {
	qreal extra = this->pen().width() / 2.0 + ARRAW_HEAD_SIZE;
	return this->AbstractUMLLink::boundingRect().adjusted( -extra, -extra, extra, extra );
}

void CompositionUMLLink::paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget ) {
	QPolygonF head;
	head.append( QPointF( 0.0, 0.0 ) );
	head.append( QPointF( 2.0, -1.0 ) );
	head.append( QPointF( 4.0, 0.0 ) );
	head.append( QPointF( 2.0, 1.0 ) );
	QPainterPath path;
	path.addPolygon( head );
	QTransform transform;
	transform.translate( this->line().p2().x(), this->line().p2().y() );
	transform.scale( ARRAW_HEAD_SIZE, ARRAW_HEAD_SIZE );
	transform.rotate( 180.0 -this->line().angle() );
	QTransform backup( painter->transform() );
	painter->setTransform( transform, true );
	painter->fillPath( path, QColor( "red" ) );
	painter->setTransform( backup );

	this->AbstractUMLLink::paint( painter, option, widget );
}

int CompositionUMLLink::type() const {
	return CompositionUMLLink::Type;
}
