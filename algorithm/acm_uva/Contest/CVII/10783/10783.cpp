#include <iostream>
using namespace std;

typedef unsigned int uint;

int main()
{
	uint cases, a, b;

	cin >> cases;
	for( uint i = 0; i < cases; i++ )
	{
		cin >> a >> b;
		if( a % 2 == 0 )
			a++;
		if( b % 2 == 0 )
			b--;
		cout << "Case " << i + 1 << ": " << ( ( a + b ) * ( ( b - a ) / 2 + 1 ) ) / 2 << endl;
	}

	return 0;
}
