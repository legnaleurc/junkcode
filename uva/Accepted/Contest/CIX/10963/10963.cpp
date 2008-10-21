#include <iostream>
#include <cstdlib>
using namespace std;

typedef unsigned int uint;

int main()
{
	uint cases, lines;
	int a, b, d;
	bool flag;
	
	cin >> cases;
	while( cases-- )
	{
		d = -1;
		flag = true;
		cin >> lines;
		while( lines-- )
		{
			cin >> a >> b;
			if( d < 0 )
				d = abs( a - b );
			else if( d != abs( a - b ) )
			{
				flag = false;
			}
		}
		
		cout << ( flag ? "yes" : "no" ) << '\n';
		if( cases > 0 )
			cout << '\n';
	}
	
	return 0;
}
