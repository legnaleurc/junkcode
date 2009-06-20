#include "bitmap.hpp"

#include <fstream>
#ifdef __GNUC__
# include <tr1/memory>
#else
# include <memory>
#endif

int main( int argc, char * argv[] ) {
	using namespace std;
	using namespace pwc94u;

	tr1::shared_ptr< Image > img( new Bitmap );

	ifstream fin( argv[1], ios::binary );
	img->load( fin );
	fin.close();

	img->equalize2();

	ofstream fout( argv[2], ios::binary );
	img->save( fout );
	fout.close();

	return 0;
}
