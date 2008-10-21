//http://acm.uva.es/p/v100/10042.html
#include <iostream>
#include <sstream>
#include <algorithm>
using namespace std;

typedef unsigned int uint;

struct Array
{
	bool *content;
	uint size;
};

uint sum( uint n );
uint fac( uint n );

int main()
{
	Array prime;
	uint cases, n;

	prime.size = 105000;
	prime.content = new bool[prime.size];
	fill( prime.content, prime.content + prime.size, true );
	prime.content[0] = prime.content[1] = false;
	for( uint i = 2; ( i * i ) < prime.size; i++ )
		if( prime.content[i] )
			for( uint j = i; i * j < prime.size; j++ )
				if( prime.content[i*j] )
					prime.content[i*j] = false;

	cin >> cases;
	while( cases-- )
	{
		cin >> n;

		do{
			n++;
			if( n < prime.size && prime.content[n] )
				continue;
			if( sum( n ) == fac( n ) )
			{
				cout << n << endl;
				break;
			}
		}while( true );
	}

	delete[] prime.content;

	return 0;
}

uint sum( uint n )
{
	uint result = 0;
	ostringstream oss;

	oss << n;
	for( uint i = 0; i < oss.str().length(); i++ )
	{
		result += oss.str().at( i ) - '0';
	}

	return result;
}

uint fac( uint n )
{
	uint result = 0;
	ostringstream oss;
	bool flag = false;

	for( uint i = 2; i * i <= n; i++ )
	{
		while( n % i == 0 )
		{
			n /= i;
			oss << i;
			flag = true;
		}
	}
	if( n != 1 )
		oss << n;
	for( uint i = 0; i < oss.str().length(); i++ )
	{
		result += oss.str().at( i ) - '0';
	}

	return flag ? result : 0;
}
