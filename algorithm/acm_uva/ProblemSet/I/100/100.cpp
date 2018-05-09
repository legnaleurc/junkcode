#include <iostream>
#include <algorithm>
using std::cin;
using std::max;
using std::swap;
using std::cout;
using std::endl;

typedef unsigned int uint;

uint cycle( uint n );

int main()
{
	uint i, j, maximum;
	bool flag;
	
	while( cin >> i >> j )
	{
		if( flag = ( i > j ) )
			swap( i, j );
		
		maximum = cycle( i );
		
		for( uint k = i + 1; k <= j; k++ )
		{
			maximum = max( maximum, cycle( k ) );
		}
		
		if( flag )
			swap( i, j );
		
		cout << i << " " << j << " " << maximum << endl;
	}
	
	return 0;
}

uint cycle( uint n )
{
	uint counter = 1;
	
	while( n != 1 )
	{
		counter++;
		if( n % 2 == 1 )
			n = 3 * n + 1;
		else
			n = n / 2;
	}
	
	return counter;
}
