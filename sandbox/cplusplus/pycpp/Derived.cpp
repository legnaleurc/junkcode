#include "Derived.hpp"

namespace {

	boost::shared_ptr< Base > creator( const std::string & ) {
		return boost::shared_ptr< Base >( new Derived );
	}

	const bool INITIALIZE = Base::signin( "Derived", creator );

}

Derived::Derived(): name_() {
}

void Derived::setName( const std::string & name ) {
	this->name_ = name;
}

std::string Derived::getName() const {
	return this->name_;
}
