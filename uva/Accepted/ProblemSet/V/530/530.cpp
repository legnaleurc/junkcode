#include <iostream>
#include <algorithm>
using namespace std;

int main()
{
	unsigned long long int m,n, result;
	while( true )
	{
		cin >> n >> m;
		if( n==0 && m==0 )
			break;
		
		m = min( m, n-m );
		
		result = 1;
		for( unsigned long long int i = 0; i < m; i++ )
		{
			result *= n - i;
			result /= i + 1;
		}
		
		cout << result << endl;
	}

	return 0;
}
