#include "sourcelist.hpp"
#include <boost/regex.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <set>
#include <vector>

using namespace std;
using namespace boost;

static const regex SOURCE( "deb(\\-src)?\\s+([\\w\\d\\.\\-\\?_:/%]+)\\s+([\\w\\-]+)\\s+([\\w\\-\\s]+)" );

Source::Source() : source( false ) {}

ostream & operator <<( ostream & out, const Source & that ) {
	out << "is source? " << ( that.source ? "true" : "false" ) << endl;
	out << "site: " << that.site << endl;
	out << "part: " << that.part << endl;
	out << "sections:";
	for( std::set< std::string >::const_iterator it = that.sections.begin(); it != that.sections.end(); ++it ) {
		out << " " << *it;
	}
	out << endl;
	return out;
}

SourceList::SourceList() {}

SourceList::SourceList( const std::string & sources ) {
		ifstream fin( sources.c_str() );
		if( fin.is_open() ) {
			string line;
			while( getline( fin, line ) ) {
				smatch result;
				if( regex_match( line, result, SOURCE ) ) {
					Source source;

					if( result[1] != "" ) {
						source.source = true;
					}
					source.site = result[2];
					source.part = result[3];

					istringstream sin( result[4] );
					while( sin >> line ) {
						source.sections.insert( line );
					}
					this->list_.push_back( source );
					cout << source << endl;
				}
			}
			fin.close();
		} else {
			// throw;
		}
}
