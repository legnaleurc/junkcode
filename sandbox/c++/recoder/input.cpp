#include <iostream>
#include <sstream>
#include "input.hpp"

Input::Input( const char * method ): method( method ), args() {
}

Input::~Input() {
	std::cout << this->method << "(";
	std::list< std::string >::const_iterator it = this->args.begin();
	std::cout << *it++;
	for( ; it != this->args.end(); ++it ) {
		std::cout << "," << *it;
	}
	std::cout << ");" << std::endl;
}

Input & Input::operator <<( bool b ) {
	this->args.push_back( b ? "true" : "false" );
	return *this;
}

Input & Input::operator <<( int i ) {
	std::ostringstream sout;
	sout << i;
	this->args.push_back( sout.str() );
	return *this;
}

Input & Input::operator <<( double d ) {
	std::ostringstream sout;
	sout << d;
	this->args.push_back( sout.str() );
	return *this;
}

Input & Input::operator <<( const std::string & s ) {
	this->args.push_back( s );
	return *this;
}
