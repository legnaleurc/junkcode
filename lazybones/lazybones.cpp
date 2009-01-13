// -*- language: c++ -*-

// tr1
// #include <tr1/regex>
// #include <boost/regex.hpp>
// namespace std {
// 	namespace tr1 = ::boost;
// }

// stl
#include <iostream>
#include <fstream>
#include <list>
#include <string>
#include <stdexcept>

// c
#include <cstdlib>


typedef std::list< std::string > ls;

std::string getRepo( int op ) {
	using namespace std;
	ls repo;
	if( op & 1 ) {
		repo.push_back( "main" );
	}
	if( op & 2 ) {
		repo.push_back( "contrib" );
	}
	if( op & 4 ) {
		repo.push_back( "non-free" );
	}
	string result;
	if( !repo.empty() ) {
		ls::const_iterator it = repo.begin();
		result = *it;
		for( ; it != repo.end(); ++it ) {
			result = string( " " ) + *it;
		}
	}
	return result;
}

void setSourcesList( int option ) {
	using namespace std;

// 	static const tr1::regex COMMENT( "#.*$" );
// 	static const tr1::regex SOURCE( "deb(\\-src)?\\s+([\\w\\d\\.\\-:/%]+)\\s+([\\w\\-]+)\\s+([\\w\\-\\s]+)" );
	
	int stable = option % 10;
	option /= 10;
	int testing = option % 10;
	option /= 10;
	int unstable = option % 10;
	option /= 10;
	int experimental = option % 10;
	
	clog << "backuping /etc/apt/sources.list ..." << endl;
	if( system( "cp /etc/apt/sources.list /etc/apt/sources.list.backup" ) != 0 ) {
		throw std::runtime_error( "backup error" );
	}
	clog << "sources.list backup succeed." << endl;
	
	ofstream fout( "/etc/apt/sources.list" );
// 	vs sourcesList;
// 	string temp;

	if( !fout.is_open() ) {
		throw std::runtime_error( "open sources.list error" );
	} else {
		if( stable ) {
			fout << "deb http://www.twaren.net/debian/ stable " << getRepo( stable ) << endl;
			fout << "deb-src http://www.twaren.net/debian/ stable " << getRepo( stable ) << endl;
		}
		if( testing ) {
			fout << "deb http://www.twaren.net/debian/ testing " << getRepo( stable ) << endl;
			fout << "deb-src http://www.twaren.net/debian/ testing " << getRepo( stable ) << endl;
		}
		if( unstable ) {
			fout << "deb http://www.twaren.net/debian/ unstable " << getRepo( stable ) << endl;
			fout << "deb-src http://www.twaren.net/debian/ unstable " << getRepo( stable ) << endl;
		}
		if( experimental ) {
			fout << "deb http://www.twaren.net/debian/ experimental " << getRepo( stable ) << endl;
			fout << "deb-src http://www.twaren.net/debian/ experimental " << getRepo( stable ) << endl;
		}
	}
	
	fout.close();
}

int main() {
	using namespace std;

	int input;

	// choose distribution
	// choose section
	// choose mirror
	cout << "Which distributions and respostries do you want?\n"
		"first: experimental\tsecond: unstable\tthird: testing\tforth: stable\n"
		"0: none\t1: main\t2: contrib\t4: non-free" << endl;
	cin >> input;

	// change respository
	setSourcesList( input );

	// change grub list
	system( "./grub.sh" );

	// remove packages that not needed
	system( "./purge.sh" );

	system( "aptitude update && aptitude dist-upgrade -yR" );
	return 0;
}
