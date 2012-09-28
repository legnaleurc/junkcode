#include "portprivate.hpp"

#include <algorithm>

Port::Private::Private( AbstractUMLItem * item ):
item( item ),
position(),
links() {
}

Port::Port( AbstractUMLItem * item ):
p_( new Private( item ) ) {
}

void Port::addLink( AbstractUMLLink * link ) {
	this->p_->links.append( link );
}

QPointF Port::getScenePosition() const {
	return this->p_->item->mapToScene( this->p_->position );
}

void Port::setPosition( const QPointF & itemPosition ) {
	this->p_->position = itemPosition;
}

void Port::updateLinks() {
	std::for_each( this->p_->links.begin(), this->p_->links.end(), []( AbstractUMLLink * link )->void {
		link->updatePosition();
	} );
}
