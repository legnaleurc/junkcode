#ifndef DERIVED_HPP
#define DERIVED_HPP

#include "Base.hpp"

class Derived : public Base {
public:
	Derived();

	virtual void setName( const std::string & name );
	virtual std::string getName() const;
private:
	std::string name_;
};

#endif
