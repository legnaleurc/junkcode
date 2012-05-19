#include "valueerror_p.hpp"

using bi::ValueError;

ValueError::Private::Private( const std::string & msg ): msg( msg ) {
}

ValueError::ValueError( const std::string & msg ): p_( std::make_shared< Private >( msg ) ) {
}

ValueError::~ValueError() throw() {
}

const char * ValueError::what() const throw() {
	return this->p_->msg.c_str();
}
