#include <iostream>
using namespace std;

int main()
{
	double n;

	while( cin >> n )
	{
		if( n < 0.0 )
			break;

		cout.precision( 11 );
		cout << ( ( n == 1.0 ) ? ( 0 ) : ( n * 25.0 ) ) << "%" << endl;
	}

	return 0;
}
