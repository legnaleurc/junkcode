#include <iostream>
#include <cmath>

using std::cin;
using std::cout;
using std::ios;

struct Coordinate
{
	double x;
	double y;
};

double dist( Coordinate a, Coordinate b );

int main()
{
	unsigned int n;
	Coordinate gopher, dog, hole;

	while( cin >> n )
	{
		cin >> gopher.x >> gopher.y >> dog.x >> dog.y;
		bool flag = false;
		while( n-- )
		{
			cin >> hole.x >> hole.y;
			if( !flag && ( 2.0 * dist( gopher, hole ) <= dist( dog, hole ) ) )
			{
				flag = true;
			}
		}
		cout.setf( ios::fixed );
		cout.precision( 3 );
		if( flag )
			cout << "The gopher can escape through the hole at (" << hole.x << ',' << hole.y << ").\n";
		else
			cout << "The gopher cannot escape.\n";
	}

	return 0;
}

double dist( Coordinate a, Coordinate b )
{
	return sqrt( pow( fabs( a.x - b.x ), 2.0 ) + pow( fabs( a.y - b.y ), 2.0 ) );
}
