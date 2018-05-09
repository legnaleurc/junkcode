#include <iostream>
#include <cmath>

using std::cin;
using std::cout;

int main()
{
	int N, n;
	
	while( true )
	{
		cin >> N;
		if( N == 0 )
			break;
		
		n = static_cast<int>( ceil( sqrt( static_cast<double>( N ) ) ) );
		if( n % 2 == 0 )
		{
			if( N < n * n - n + 1 )
				cout << N - ( n - 1 ) * ( n - 1 ) << ' ' << n << '\n';
			else
				cout << n << ' ' << n * n - N + 1 << '\n';
		}
		else
		{
			if( N < n * n - n + 1 )
				cout << n << ' ' << N - ( n - 1 ) * ( n - 1 ) << '\n';
			else
				cout << n * n - N + 1 << ' ' << n << '\n';
		}
	}
	
	return 0;
}
