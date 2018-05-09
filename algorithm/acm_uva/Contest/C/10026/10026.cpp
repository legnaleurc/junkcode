//http://acm.uva.es/p/v100/10026.html
#include <iostream>
#include <algorithm>
using namespace std;

typedef unsigned int uint;

struct Order
{
	double cost;
	uint index;
};

struct Array
{
	Order *data;
	uint size;
};

inline bool compare( Order a, Order b ) {return a.cost > b.cost;}

int main()
{
	uint cases;
	double days, fine;
	Array jobs;
	
	cin >> cases;
	while( cases-- )
	{
		cin >> jobs.size;
		jobs.data = new Order[jobs.size];
		for( uint i = 0; i < jobs.size; i++ )
		{
			cin >> days >> fine;
			jobs.data[i].cost = fine / days;
			jobs.data[i].index = i + 1;
		}
		
		sort( jobs.data, jobs.data + jobs.size, compare );
		
		for( uint i = 0; i < jobs.size; i++ )
		{
			cout << jobs.data[i].index;
			if( i < jobs.size - 1 )
				cout << " ";
		}
		cout << endl;
		if( cases > 0 )
			cout << endl;
		
		delete[] jobs.data;
	}
	return 0;
}

/*
這個問題只要找出每件工作的成本，也就是罰金和天數的比例，照降冪排序即可。
*/
