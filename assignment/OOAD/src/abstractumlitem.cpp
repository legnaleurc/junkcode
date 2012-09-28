#include "abstractumlitemprivate.hpp"
#include "abstractumllink.hpp"

#include <QtGui/QMenu>
#include <QtGui/QGraphicsScene>
#include <QtGui/QPainter>

#include <cmath>

namespace {

	std::map< int, std::function< AbstractUMLItem * ( QGraphicsItem * ) > > & getFactory() {
		static std::map< int, std::function< AbstractUMLItem * ( QGraphicsItem * ) > > f;
		return f;
	}

}

AbstractUMLItem * AbstractUMLItem::create( int id, QGraphicsItem * parent ) {
	std::map< int, std::function< AbstractUMLItem * ( QGraphicsItem * ) > >::const_iterator it = getFactory().find( id );
	if( it == getFactory().end() ) {
		return NULL;
	}
	AbstractUMLItem * item = it->second( parent );
	QRectF bound = item->boundingRect();
	item->p_->topPort->setPosition( QPointF( bound.center().x(), bound.top() ) );
	item->p_->bottomPort->setPosition( QPointF( bound.center().x(), bound.bottom() ) );
	item->p_->leftPort->setPosition( QPointF( bound.left(), bound.center().y() ) );
	item->p_->rightPort->setPosition( QPointF( bound.right(), bound.center().y() ) );
	return item;
}

bool AbstractUMLItem::signup( int id, std::function< AbstractUMLItem * ( QGraphicsItem * ) > creator ) {
	return getFactory().insert( std::make_pair( id, creator ) ).second;
}

AbstractUMLItem::Private::Private( AbstractUMLItem * host ):
topPort( new Port( host ) ),
bottomPort( new Port( host ) ),
leftPort( new Port( host ) ),
rightPort( new Port( host ) ),
name() {
}

AbstractUMLItem::AbstractUMLItem( QGraphicsItem * parent ):
QAbstractGraphicsShapeItem( parent ),
p_( new Private( this ) ) {
	this->setFlags( QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemSendsGeometryChanges );

	QPen pen( this->pen() );
	pen.setColor( "red" );
	this->setPen( pen );
}

void AbstractUMLItem::paint( QPainter * painter, const QStyleOptionGraphicsItem * /*option*/, QWidget * /*widget*/ ) {
	if( this->isSelected() && !this->group() ) {
		const QRectF rect( this->boundingRect() );
		const QPointF center( rect.center() );
		const qreal SIZE = 4.0;
		QPainterPath path;
		path.addRect( center.x() - SIZE / 2, rect.top(), SIZE, SIZE );
		path.addRect( center.x() - SIZE / 2, rect.bottom() - SIZE, SIZE, SIZE );
		path.addRect( rect.left(), center.y() - SIZE / 2, SIZE, SIZE );
		path.addRect( rect.right() - SIZE, center.y() - SIZE / 2, SIZE, SIZE );
		painter->fillPath( path, QColor( "red" ) );
	}
	this->p_->topPort->updateLinks();
	this->p_->bottomPort->updateLinks();
	this->p_->leftPort->updateLinks();
	this->p_->rightPort->updateLinks();
}

int AbstractUMLItem::type() const {
	return AbstractUMLItem::Type;
}

const QString & AbstractUMLItem::getName() const {
	return this->p_->name;
}

void AbstractUMLItem::setName( const QString & name ) {
	this->p_->name = name;
	this->update();
}

Port * AbstractUMLItem::getAttachPort( const QPointF & scenePosition ) const {
	QRectF rect( this->boundingRect() );
	QPointF center( rect.center() );
	QPointF diff( this->mapFromScene( scenePosition ) - center );
	bool isVertical = ( fabs( diff.x() ) * 2 ) / rect.width() < ( fabs( diff.y() ) * 2 ) / rect.height();
	if( isVertical ) {
		if( diff.y() < 0.0 ) {
			return this->p_->topPort.get();
		} else {
			return this->p_->bottomPort.get();
		}
	} else {
		if( diff.x() < 0.0 ) {
			return this->p_->leftPort.get();
		} else {
			return this->p_->rightPort.get();
		}
	}
}
