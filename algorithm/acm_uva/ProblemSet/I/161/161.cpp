#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>
using namespace std;

typedef unsigned int uint;

struct Array
{
	uint data[101];
	uint size;
};

bool check( uint n, uint k );
bool strict( Array& v, uint u );
string form( uint time );

int main()
{
	uint temp, minimum, c = 0;
	Array v;
	v.size = 0;

	while( cin >> temp )
	{
		v.data[v.size++] = temp;

		if( v.data[v.size-1] == 0 && c < 2 )
		{
			v.size--;
			if( v.size == 0 )
			{
				c++;
				continue;
			}
			minimum = *min_element( v.data, v.data + v.size ) * 2;
			while( minimum <= 18000 && !strict( v, minimum ) )
				minimum++;

			cout << form( minimum ) << endl;
			v.size = 0;
			c = 0;
		}
		if( c >= 2 )
			break;
	}

	return 0;
}

bool check( uint n, uint k )
{
	return ( ( k % ( 2 * n ) ) < ( n - 5 ) );
}

bool strict( Array& v, uint u )
{
	for( uint i = 0; i < v.size; i++ )
	{
		if( !check( v.data[i], u ) )
			return false;
	}

	return true;
}

string form( uint time )
{
	ostringstream oss;
	uint second = time % 60;
	time = ( time - second ) / 60;
	uint minute = time % 60;
	time = ( time - minute ) / 60;
	uint hour = time % 60;

	if( hour == 5 && ( minute > 0 || second > 0 ) )
		oss << "Signals fail to synchronise in 5 hours";
	else
	{
		oss.fill( '0' );
		oss.width( 2 );
		oss << hour;
		oss << ":";
		oss.fill( '0' );
		oss.width( 2 );
		oss << minute;
		oss << ":";
		oss.fill( '0' );
		oss.width( 2 );
		oss << second;
	}

	return oss.str();
}
