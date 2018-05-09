#include <iostream>
#include <algorithm>
using std::fill_n;
using std::cin;
using std::cout;
using std::endl;

int main()
{
	bool* prime = new bool[18500000];
	unsigned int pair[100000];
	unsigned int s = 0;

	fill_n( prime, 18500000, true );

	prime[0] = prime[1] = false;

	for( unsigned int i = 2; i * i < 18500000; i++ )
	{
		if( prime[i] )
		{
			for( unsigned int j = i; j * i < 18500000; j++ )
			{
				if( prime[j*i] )
				{
					prime[j*i] = false;
				}
			}
		}
	}

	for( unsigned int i = 0; s < 100000; i++ )
	{
		if( prime[i] && prime[i-2] )
		{
			pair[s++] = i - 1;
		}
	}

	delete [] prime;
	prime = NULL;

	while( cin >> s )
	{
		cout << "(" << pair[s-1] - 1 << ", " << pair[s-1] + 1 << ")" << endl;
	}

	return 0;
}
