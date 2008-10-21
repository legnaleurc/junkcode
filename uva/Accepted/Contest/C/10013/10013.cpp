//http://acm.uva.es/p/v100/10013.html
#include <iostream>
using namespace std;

typedef unsigned int uint;

struct Array
{
	uint *data;
	uint size;
};

int main()
{
	Array sum;
	uint cases, a, b;
	
	cin >> cases;
	
	while( cases-- )
	{
		cin >> sum.size;
		sum.data = new uint[sum.size];
		for( uint i = 0; i < sum.size; i++ )
		{
			cin >> a >> b;
			sum.data[i] = a + b;
		}
		for( uint i = 0; i < sum.size; i++ )	//Deal with carry out.
		{
			if( sum.data[sum.size - 1 - i] >= 10 )
			{
				sum.data[sum.size - 1 - i - 1]++;
				sum.data[sum.size - 1 - i] -= 10;
			}
		}
		for( uint i = 0; i < sum.size; i++ )
		{
			cout << sum.data[i];
		}
		cout << endl;
		if( cases > 0 )
			cout << endl;
		
		delete[] sum.data;
	}
	
	return 0;
}
