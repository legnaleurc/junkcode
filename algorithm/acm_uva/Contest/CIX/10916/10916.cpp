//http://acm.uva.es/p/v109/10916.html
#include <iostream>
#include <cmath>
using namespace std;

int main()
{
	int input, factstone;
	double power, bit, temp_sum;

	while( cin >> input )
	{
		if( input == 0 )
			break;

		power = floor( ( static_cast<double>( input - 1940 ) ) / 10.0 );
		bit = pow( 2, power );
		
		bit *= log10( 2.0 );
		factstone = 2;
		temp_sum = log10( static_cast<double>( factstone ) );
		while( temp_sum <= bit )
		{
			temp_sum += log10( static_cast<double>( ++factstone ) );
		}

		cout << --factstone << endl;
	}
}