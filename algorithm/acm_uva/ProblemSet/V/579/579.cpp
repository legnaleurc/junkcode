#include <iostream>
#include <algorithm>
using namespace std;

int main()
{
	double h, m, degree;

	while( true )
	{
		cin >> h;
		cin.ignore();
		cin >> m;
		if( h == 0.0 && m == 0.0 )
			break;

		h = h * 30.0 + m * 0.5;
		m = m * 6.0;
		degree = ( m > h ) ? ( m - h ) : ( h - m );
		degree = min( degree, 360 - degree );

		cout.setf( ios::fixed );
		cout.precision( 3 );
		cout << degree << endl;
	}
	return 0;
}
