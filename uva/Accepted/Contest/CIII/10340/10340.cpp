#include <iostream>
#include <string>
using namespace std;

typedef unsigned int uint;

int main()
{
	string a,b;

	while( cin >> a >> b )
	{
		const uint al = a.length(), bl = b.length();
		
		uint j = 0;
		for( uint i = 0; i < bl; i++ )
		{
			if( a[j] == b[i] )
				j++;
			if( j == al )
				break;
		}
		if( j == al )
		{
			cout << "Yes" << endl;
		}
		else
		{
			cout << "No" << endl;
		}
	}
	
	return 0;
}
