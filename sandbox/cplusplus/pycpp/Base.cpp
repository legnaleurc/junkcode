#include "Base.hpp"

#include <map>

namespace {

	std::map< std::string, boost::function< boost::shared_ptr< Base > ( const std::string & ) > > & factory() {
		static std::map< std::string, boost::function< boost::shared_ptr< Base > ( const std::string & ) > > tmp;
		return tmp;
	}

}

boost::shared_ptr< Base > Base::create( const std::string & type ) {
	std::map< std::string, boost::function< boost::shared_ptr< Base > ( const std::string & ) > >::const_iterator it = factory().find( type );
	if( it == factory().end() ) {
		return boost::shared_ptr< Base >();
	} else {
		return it->second( type );
	}
}

bool Base::signin( const std::string & type, boost::function< boost::shared_ptr< Base > ( const std::string & ) > creator ) {
	std::map< std::string, boost::function< boost::shared_ptr< Base > ( const std::string & ) > >::const_iterator it = factory().find( type );
	if( it == factory().end() ) {
		factory().insert( std::make_pair( type, creator ) );
		return true;
	} else {
		return false;
	}
}

Base::~Base() {
}
