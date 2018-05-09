//http://acm.uva.es/p/v100/10041.html
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
using namespace std;

int main()
{
	int cases, num, temp, index, sum;
	vector<int> v;
	
	cin >> cases;
	while( cases-- )
	{
		cin >> num;
		while( num-- )
		{
			cin >> temp;
			v.push_back( temp );
		}
		sort( v.begin(), v.end() );
		index = v.size() / 2;
		sum = 0;
		for( size_t i = 0; i < v.size(); i++ )
		{
			sum += abs( v[index] - v[i] );
		}
		cout << sum << endl;
		v.clear();
	}
	
	return 0;
}

/*
與各數字之差的總和為最小之數即中位數
*/
