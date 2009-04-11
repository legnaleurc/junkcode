#include "bitmap.hpp"

#include <fstream>

int main( int argc, char * argv[] ) {
	using namespace std;
	using namespace pwc94u;

	Image img;

	ifstream fin( argv[1], ios::binary );
	img.load( fin );
	fin.close();

	img.equalize();

	ofstream fout( argv[2], ios::binary );
	img.save( fout );
	fout.close();

	return 0;
}
