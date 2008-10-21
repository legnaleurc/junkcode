#include <iostream>
using namespace std;

int main()
{
	int n, sum;

	cout << "PERFECTION OUTPUT" << endl;
	while( cin >> n )
	{
		if( n == 0 )
			break;

		sum = 1;
		for( int i = 2; i * i <= n; i++ )
		{
			if( n % i == 0 )
			{
				if( i * i != n )
					sum += n / i;
				sum += i;
			}
		}
		if( n == 1 )
			sum--;

		cout.width( 5 );
		cout.setf( ios::right );
		cout << n;
		cout << "  ";
		if( n < sum )
		{
			cout << "ABUNDANT";
		}
		else if( n > sum )
		{
			cout << "DEFICIENT";
		}
		else
		{
			cout << "PERFECT";
		}
		cout << endl;
	}
	cout << "END OF OUTPUT" << endl;
	return 0;
}
