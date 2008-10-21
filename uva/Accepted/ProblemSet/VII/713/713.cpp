#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

typedef unsigned int uint;

int main()
{
	uint cases;
	string a, b;
	
	cin >> cases;
	while( cases-- )
	{
		cin >> a >> b;
		if( a.length() < b.length() )
			a.swap( b );
		uint carry = 0, i;
		for( i = 0; i < b.length(); i++ )
		{
			a[i] = a[i] - '0' + b[i] - '0' + carry;
			if( a[i] >= 10 )
			{
				a[i] -= 10;
				carry = 1;
			}
			else
			{
				carry = 0;
			}
			a[i] += '0';
		}
		for( ; i < a.length(); i++ )
		{
			a[i] = a[i] - '0' + carry;
			if( a[i] >= 10 )
			{
				a[i] -= 10;
				carry = 1;
			}
			else
			{
				carry = 0;
			}
			a[i] += '0';
		}
		if( carry == 1 )
			a += "1";
		
		while( a[0] == '0' )
			a.erase( a.begin() );
		
		cout << a << endl;
	}
	return 0;
}
