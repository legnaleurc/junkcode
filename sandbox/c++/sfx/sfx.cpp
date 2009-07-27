#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <cstdlib>

const char * OUT_FILE = "/tmp/injection";

int main( int argc, char * argv[] ) {
	using namespace std;
	typedef char Byte;
	typedef vector< Byte > ByteArray;
	ifstream fin( argv[0], ios::binary );

	if( fin.is_open() ) {
		ByteArray binary;
		copy( istreambuf_iterator< Byte >( fin ), istreambuf_iterator< Byte >(), back_inserter( binary ) );
		fin.close();

		string magic( "!_METAMAGIC__" );
		magic[0] = '_';
		ByteArray::iterator result = search( binary.begin(), binary.end(), magic.begin(), magic.end() );
		if( result == binary.end() ) {
			cerr << "Can not find piece!" << endl;
			return 1;
		} else {
			advance( result, magic.length() );
			ofstream fout( OUT_FILE, ios::binary );
			if( fout.is_open() ) {
				copy( result, binary.end(), ostreambuf_iterator< Byte >( fout ) );
				fout.close();
				ByteArray().swap( binary );
				system( ( string( "chmod a+x " ) + OUT_FILE + " && " + OUT_FILE ).c_str() );
			} else {
				cerr << "Can not write `" << OUT_FILE << "\'!" << endl;
				return 1;
			}
		}
	} else {
		cerr << "Can not open " << argv[0] << "!" << endl;
		return 1;
	}

	return 0;
}
