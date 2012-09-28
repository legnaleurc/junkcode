#include "associationumllink.hpp"

namespace {

	const bool REGISTERED = AbstractUMLLink::signup( 0, []( Port * begin, Port * end )->AbstractUMLLink * {
		return new AssociationUMLLink( begin, end );
	} );

}

AssociationUMLLink::AssociationUMLLink( Port * begin, Port * end, QGraphicsItem * parent ):
AbstractUMLLink( begin, end, parent ) {
}

int AssociationUMLLink::type() const {
	return AssociationUMLLink::Type;
}
