#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

typedef unsigned int uint;

int main()
{
	string a, b;
	uint o[26], p[26];
	
	while( getline( cin, a ) )
	{
		getline( cin, b );
		
		fill_n( o, 26, 0 );
		fill_n( p, 26, 0 );
		for( uint i = 0; i < a.length(); i++ )
		{
			o[a[i]-'a']++;
		}
		for( uint i = 0; i < b.length(); i++ )
		{
			p[b[i]-'a']++;
		}
		
		for( uint i = 0; i < 26; i++ )
		{
			const uint minimum = min( o[i], p[i] );
			
			for( int j = 0; j < minimum; j++ )
			{
				cout << static_cast<char>( i + 'a' );
			}
		}
		cout << endl;
	}
	return 0;
}
