//http://acm.uva.es/p/v100/10038.html
#include <iostream>
#include <vector>
#include <cstdlib>
#include <algorithm>
using namespace std;

int main()
{
	int cases;
	
	while( cin >> cases )
	{
		int temp;
		vector<int> v;
		bool flag = true;
		
		while( cases-- )
		{
			cin >> temp;
			v.push_back( temp );
		}
		
		for( int i = 0; i < v.size() - 1; i++ )	//Create subtraction table.
		{
			v[i] = abs( v[i] - v[i+1] );
		}
		v.pop_back();
		
		sort( v.begin(), v.end() );
		
		for( int i = 0; i < v.size(); i++ )
		{
			if( v[i] != i + 1 )
			{
				flag = false;
				break;
			}
		}
		if( flag )
			cout << "Jolly" << endl;
		else
			cout << "Not jolly" << endl;
	}
	
	return 0;
}
