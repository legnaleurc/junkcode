#include "generalizationumllink.hpp"

#include <QtGui/QPainter>

#include <cmath>

namespace {

	const bool REGISTERED = AbstractUMLLink::signup( 1, []( Port * begin, Port * end )->AbstractUMLLink * {
		return new GeneralizationUMLLink( begin, end );
	} );

	const qreal ARROW_HEAD_SIZE = 20;
	const double PI = ::acos( -1.0 );

}

GeneralizationUMLLink::GeneralizationUMLLink( Port * begin, Port * end, QGraphicsItem * parent ):
AbstractUMLLink( begin, end, parent ) {
}

QRectF GeneralizationUMLLink::boundingRect() const {
	qreal extra = ( this->pen().widthF() + ARROW_HEAD_SIZE ) / 2.0;
	return this->AbstractUMLLink::boundingRect().adjusted( -extra, -extra, extra, extra );
}

void GeneralizationUMLLink::paint( QPainter * painter, const QStyleOptionGraphicsItem * /*option*/, QWidget * /*parent*/ ) {
	QPolygonF head;
	head.append( QPointF( 0.0, 0.0 ) );
	head.append( QPointF( ::cos( PI / 6 ), -::sin( PI / 6 ) ) );
	head.append( QPointF( ::cos( -PI / 6 ), -::sin( -PI / 6 ) ) );

	QPainterPath path;
	path.addPolygon( head );

	QTransform transform;
	transform.translate( this->line().p2().x(), this->line().p2().y() );
	transform.scale( ARROW_HEAD_SIZE, ARROW_HEAD_SIZE );
	transform.rotate( 180.0 - this->line().angle() );

	QTransform backup( painter->transform() );
	painter->setTransform( transform, true );
	painter->fillPath( path, QColor( "yellow" ) );
	painter->setPen( this->pen() );
	painter->drawPolygon( head );
	painter->setTransform( backup );

	qreal triangleHeight = ::sqrt( 3.0 ) / 2.0;
	QPointF realP2( transform.map( QLineF( QPointF( 0.0, 0.0 ), QPointF( triangleHeight, 0.0 ) ) ).p2() );
	painter->drawLine( this->line().p1(), realP2 );
}

int GeneralizationUMLLink::type() const {
	return GeneralizationUMLLink::Type;
}
