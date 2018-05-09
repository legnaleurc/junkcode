#include <iostream>
#include <algorithm>
#include <cstdlib>
using namespace std;

int main()
{
	int cases, stores, pos[20];
	cin >> cases;
	while( cases-- )
	{
		cin >> stores;
		for( int i = 0; i < stores; ++i )
			cin >> pos[i];

		sort( pos, pos + stores );

		cout << 2 * ( pos[stores - 1] - pos[0] ) << endl;
	}
	return 0;
}
