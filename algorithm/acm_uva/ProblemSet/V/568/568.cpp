#include <iostream>
using namespace std;

int main()
{
	int input;
	long long int result;
	
	while( cin >> input )
	{
		result = 1;
		for( int i = 1; i <= input; i++ )
		{
			result *= i;
			while( result % 10 == 0 )
			{
				result /= 10;
			}
			if( result > 100000 )
			{
				result %= 100000;
			}
		}
		cout.width( 5 );
		cout.setf( ios::right );
		cout << input;
		cout << " -> " << result % 10 << endl;
	}
	
	return 0;
}
