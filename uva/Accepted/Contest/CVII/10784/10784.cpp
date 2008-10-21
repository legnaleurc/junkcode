#include <iostream>
#include <cmath>
using namespace std;

int main()
{
	long long int N;
	unsigned int i = 0;
	
	while(cin >> N)
	{
		if(N == 0)
			break;
		cout.precision(15);
		cout << "Case " << ++i << ": " << ceil((3.0 + sqrt(9.0+8.0*N)) / 2.0) << endl;
	}
	
	return 0;
}
