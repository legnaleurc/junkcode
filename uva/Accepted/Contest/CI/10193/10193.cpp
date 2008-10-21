#include <iostream>
#include <string>
#include <cmath>
using namespace std;

typedef unsigned int uint;

uint GCD( uint p, uint q );
uint btod( string s );

int main()
{
	uint cases;
	string s1, s2;

	cin >> cases;
	for( uint i = 0; i < cases; i++ )
	{
		cin >> s1 >> s2;
		cout << "Pair #" << i + 1 << ": ";
		if( GCD( btod( s1 ), btod( s2 ) ) == 1 )
			cout << "Love is not all you need!" << endl;
		else
			cout << "All you need is love!" << endl;
	}

	return 0;
}

uint GCD( uint p, uint q )
{
	if( p % q != 0 )
		return GCD( q, p % q );
	else
		return q;
}

uint btod( string s )
{
	uint result = 0;

	for( uint i = 0; i < s.length(); i++ )
	{
		result += ( *( s.rbegin() + i ) - '0' ) * static_cast<uint>( pow( static_cast<double>( 2 ), static_cast<double>( i ) ) );
	}

	return result;
}
