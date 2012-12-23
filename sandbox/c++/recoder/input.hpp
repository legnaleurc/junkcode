#ifndef INPUT_HPP
#define INPUT_HPP

#include <string>
#include <list>

class Input {
public:
	Input( const char * method );
	~Input();

	Input & operator <<( bool b );
	Input & operator <<( int i );
	Input & operator <<( double d );
	Input & operator <<( const std::string & s );
private:
	std::string method;
	std::list< std::string > args;
};

#define xInput Input( __FUNCTION__ )

#endif
