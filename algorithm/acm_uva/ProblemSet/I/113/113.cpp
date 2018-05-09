#include <iostream>
#include <cmath>
using namespace std;

int main()
{
	double n, p;

	while( cin >> n && cin >> p )
	{
		cout.precision( 9 );
		cout << pow( p, 1.0/n ) << endl;
	}

	return 0;
}
