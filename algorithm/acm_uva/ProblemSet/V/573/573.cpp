#include <iostream>
using namespace std;

int main()
{
	double h, u, d, f, height, rate;
	unsigned int day;

	while( cin >> h >> u >> d >> f )
	{
		if( h == 0 )
			break;

		height = u;
		rate = u * f / 100.0;
		day = 1;
		while( height <= h && height >= 0 )
		{
			height -= d;
			if( height < 0 )
				break;
			( u >= rate ) ? ( u -= rate ) : ( u = 0 );
			height += u;
			day++;
		}

		if( height < 0 )
		{
			cout << "failure on day " << day << endl;
		}
		else
		{
			cout << "success on day " << day << endl;
		}
	}

	return 0;
}