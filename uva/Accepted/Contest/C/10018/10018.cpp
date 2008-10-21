#include <iostream>
#include <algorithm>
#include <string>
using namespace std;

typedef unsigned int uint;

bool radd( string& s, uint& counter );

int main()
{
	uint cases, counter;
	string temp;
	
	cin >> cases;
	while( cases-- )
	{
		cin >> temp;
		counter = 0;

		while( radd( temp, counter ) ){}

		cout << counter << " " << temp << endl;
	}

	return 0;
}

bool radd( string& s, uint& counter )
{
	string clone( s.length(), 0 );
	uint carry = 0;

	reverse_copy( s.begin(), s.end(), clone.begin() );

	if( clone == s && counter > 0 )
		return false;
	else
	{
		counter++;
		for( int i = s.length() - 1; i >= 0; i-- )
		{
			s[i] += clone[i] - '0' + carry;
			if( s[i] > '9' )
			{
				s[i] -= 10;
				carry = 1;
			}
			else
				carry = 0;
		}
		if( carry == 1 )
			s = "1" + s;

		return true;
	}
}
