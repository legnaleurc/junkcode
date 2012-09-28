#include "abstractumllinkprivate.hpp"
#include "abstractumlitem.hpp"

#include <QtGui/QPen>

namespace {

	std::map< int, std::function< AbstractUMLLink * ( Port *, Port * ) > > & getFactory() {
		static std::map< int, std::function< AbstractUMLLink * ( Port *, Port * ) > > f;
		return f;
	}

}

AbstractUMLLink::Private::Private( Port * begin, Port * end ):
begin( begin ),
end( end ) {
}

AbstractUMLLink * AbstractUMLLink::create( int id, Port * begin, Port * end ) {
	std::map< int, std::function< AbstractUMLLink * ( Port *, Port * ) > >::const_iterator it = getFactory().find( id );
	if( it == getFactory().end() ) {
		return NULL;
	}
	return it->second( begin, end );
}

bool AbstractUMLLink::signup( int id, std::function< AbstractUMLLink * ( Port *, Port * ) > creator ) {
	return getFactory().insert( std::make_pair( id, creator ) ).second;
}

AbstractUMLLink::AbstractUMLLink( Port * begin, Port * end, QGraphicsItem * parent ):
QGraphicsLineItem( parent ),
p_( new Private( begin ,end ) ) {
	begin->addLink( this );
	end->addLink( this );

	QPen pen( this->pen() );
	pen.setColor( "red" );
	this->setPen( pen );

	this->setZValue( -1000 );
}

int AbstractUMLLink::type() const {
	return AbstractUMLLink::Type;
}

void AbstractUMLLink::updatePosition() {
	this->setLine( QLineF( this->p_->begin->getScenePosition(), this->p_->end->getScenePosition() ) );
}
