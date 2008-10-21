#include <iostream>
#include <set>
#include <vector>
#include <algorithm>
using namespace std;

const size_t MAX = 40000;

int main()
{
	int n, counter = 0;
	vector<int> v;
	set<int> used;
	bool temp[MAX];

	fill_n( temp, MAX, true );
	temp[0] = temp[1] = false;
	for( int i = 2; i * i < MAX; ++i )
		if( temp[i] )
			for( int j = i; j * i < MAX; ++j )
				if( temp[j*i] )
					temp[j*i] = false;

	for( int i = 0; i < MAX; ++i )
		if( temp[i] )
			v.push_back( i );

	while( cin >> n )
	{
		if( n == 0 )
			break;

		counter = 0;
		used.clear();
		for( int i = 0; i < n / 2; i++ )
		{
			if( find( used.begin(), used.end(), v[i] ) == used.end() && find( v.begin(), v.end(), n - v[i] ) != v.end() )
			{
				++counter;
				used.insert( n - v[i] );
			}
		}
		cout << counter << '\n';
	}

	return 0;
}
