#include <iostream>
using namespace std;

int main()
{
	long long int a, l, temp;
	int term, cases = 1;

	while( cin >> a >> l )
	{
		if( a < 0 && l < 0 )
			break;

		term = 1;
		temp = a;
		while( temp != 1 && temp <= l )
		{
			if( temp % 2 == 0 )
			{
				temp /= 2;
			}
			else
			{
				temp = 3 * temp + 1;
			}
			term++;
		}
		if( temp > l )
		{
			term--;
		}

		cout << "Case " << cases++ << ": A = " << a << ", limit = " << l << ", number of terms = " << term << endl;
	}

	return 0;
}
