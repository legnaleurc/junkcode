//http://acm.uva.es/p/v108/10812.html
#include <iostream>
using namespace std;

typedef unsigned int uint;

int main()
{
	uint cases, s, d;
	
	cin >> cases;
	while( cases-- )
	{
		cin >> s >> d;
		if( s < d || ( s + d ) % 2 == 1 )
			cout << "impossible" << endl;
		else
			cout << ( s + d ) / 2 << " " << ( s - d ) / 2 << endl;
	}
	
	return 0;
}
