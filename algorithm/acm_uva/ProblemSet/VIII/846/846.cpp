#include <iostream>
#include <cmath>
using namespace std;

typedef unsigned int uint;
typedef long long int llint;

int main()
{
	uint cases;
	double x, y;
	
	cin >> cases;
	while( cases-- )
	{
		cin >> x >> y;
		y -= x;
		
		x = sqrt( y );
		y = ceil( x );
		
		if( y * y - y <= x * x )
			cout << floor( y * 2.0 - 1.0 + 0.5 ) << endl;
		else
			cout << floor( ( y - 1.0 ) * 2.0 + 0.5 ) << endl;
	}
	
	return 0;
}
