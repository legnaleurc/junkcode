#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

typedef unsigned short int usint;
typedef unsigned int uint;

char en( char c );
char de( char c );

int main()
{
	string a, b, p;
	char temp;
	
	while( cin >> a >> b )
	{
		reverse( a.begin(), a.end() );
		reverse( b.begin(), b.end() );
		transform( a.begin(), a.end(), a.begin(), en );
		transform( b.begin(), b.end(), b.begin(), en );
		p.resize( a.length() + b.length() );
		fill( p.begin(), p.end(), 0 );
		
		for( uint i = 0; i < a.length(); i++)
		{
			for( uint j = 0; j < b.length(); j++ )
			{
				temp = a[i] * b[j];
				p[i+j] += temp % 10;
				if( p[i+j] >= 10 )
				{
					p[i+j+1] += p[i+j] / 10;
					p[i+j] = p[i+j] % 10;
				}
				p[i+j+1] += temp / 10;
			}
		}
		while( *( p.end() - 1 ) == 0 )
			p.erase( p.end() - 1 );
		transform( p.begin(), p.end(), p.begin(), de );
		reverse( p.begin(), p.end() );
		if( p.length() == 0 )
			cout << "0" << endl;
		else
			cout << p << endl;
	}
	
	return 0;
}

char en( char c )
{
	return c - '0';
}

char de( char c )
{
	return c + '0';
}
