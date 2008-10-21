//http://acm.uva.es/p/v100/10055.html
#include <iostream>
using namespace std;

typedef long long int llint;

int main()
{
	llint a,b;

	while( cin >> a >> b )
	{
		cout << ( ( a > b ) ? ( a - b ) : ( b - a ) ) << endl;
	}

	return 0;
}
