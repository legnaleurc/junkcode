#include <iostream>
#include <algorithm>
using namespace std;

inline bool compare( int a, int b ) { return a > b; }

int main()
{
	int cases, items, mono[20000];
	cin >> cases;
	while( cases-- )
	{
		cin >> items;
		for( int i = 0; i < items; ++i )
		{
			cin >> mono[i];
		}

		sort( mono, mono + items, compare );

		int sum = 0;
		for( int i = 1; i * 3 - 1 < items; ++i )
		{
			sum += mono[i*3-1];
		}

		cout << sum << endl;
	}
}
