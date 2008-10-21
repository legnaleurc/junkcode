#include <iostream>
#include <cmath>
using namespace std;

int main()
{
	double n, m;

	while( true )
	{
		cin >> n;
		if( n == 0.0 )
			break;

		m = sqrt( n );
		if( m == ceil( m ) && m == floor( m ) )
			cout << "yes" << endl;
		else
			cout << "no" << endl;
	}
	return 0;
}
