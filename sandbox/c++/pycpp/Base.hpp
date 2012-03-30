#ifndef BASE_HPP
#define BASE_HPP

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

class Base {
public:
	static boost::shared_ptr< Base > create( const std::string & type );
	static bool signin( const std::string & type, boost::function< boost::shared_ptr< Base > ( const std::string & ) > creator );

	virtual ~Base();
	virtual void setName( const std::string & publicKey ) = 0;
	virtual std::string getName() const = 0;
};

#endif
