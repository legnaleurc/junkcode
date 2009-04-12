#include "bitmap.hpp"

#include <fstream>
#include <tr1/memory>

int main( int argc, char * argv[] ) {
	using namespace std;
	using namespace pwc94u;

	tr1::shared_ptr< Image > img( new Bitmap );

	ifstream fin( argv[1], ios::binary );
	img->load( fin );
	fin.close();

	img->equalize();

	ofstream fout( argv[2], ios::binary );
	img->save( fout );
	fout.close();

	return 0;
}
