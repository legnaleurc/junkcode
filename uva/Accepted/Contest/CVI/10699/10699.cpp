//http://acm.uva.es/p/v106/10699.html
#include <iostream>
#include <vector>
using namespace std;

int main()
{
	int input, temp;
	vector<int> factor;

	while( cin >> input )
	{
		if( input == 0 )
			break;

		temp = input;
		for( int i = 2; i * i <= input; i++ )
		{
			while( temp % i == 0 )
			{
				if( factor.empty() || factor.back() != i )
				{
					factor.push_back( i );
				}
				temp /= i;
			}
		}
		if( temp != 1 )
		{
			factor.push_back( temp );
		}

		cout << input << " : " << factor.size() << endl;

		factor.clear();
	}

	return 0;
}