#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
using namespace std;

int main()
{
	int cases, size;
	string temp;
	vector< string > phone;

	cin >> cases;
	while( cases-- )
	{
		cin >> size;
		for( int i = 0; i < size; ++i )
		{
			cin >> temp;
			phone.push_back( temp );
		}

		sort( phone.begin(), phone.end() );

		bool flag = true;
		for( int i = 0; i < phone.size() - 1; ++i )
		{
			if( phone[i+1].find( phone[i] ) == 0 )
			{
				flag = false;
				break;
			}
		}

		cout << ( flag ? "YES" : "NO" ) << endl;

		phone.clear();
	}

	return 0;
}
