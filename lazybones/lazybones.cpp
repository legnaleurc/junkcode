// -*- language: c++ -*-
#include "sourcelist.hpp"

// stl
#include <iostream>
#include <fstream>
#include <list>
#include <string>
#include <stdexcept>

// c
#include <cstdlib>

typedef std::list< std::string > ls;

void setupSourcesList() {
	SourceList sourceList( "/etc/apt/sources.list" );
}

int main() {
	using namespace std;

	// change respository
	setupSourcesList();

	// change grub list
	system( "./grub.sh" );

	// remove packages that not needed
	system( "./purge.sh" );

	system( "aptitude update && aptitude dist-upgrade -yR" );
	return 0;
}
