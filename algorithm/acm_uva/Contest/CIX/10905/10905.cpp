#include <iostream>
#include <sstream>
#include <algorithm>
using namespace std;

typedef unsigned int uint;

struct Array
{
	string content[50];
	uint size;
};

bool compare( string a, string b );

int main()
{
	Array num;

	while( cin >> num.size )
	{
		if( num.size == 0 )
			break;
		for( uint i = 0; i < num.size; i++ )
		{
			cin >> num.content[i];
		}
		sort( num.content, num.content + num.size, compare );
		for( uint i = 0; i < num.size; i++ )
		{
			cout << num.content[i];
		}
		cout << endl;
	}
	
	return 0;
}

bool compare( string a, string b )
{
	uint am = 0, bm = 0;
	const uint al = a.length(), bl = b.length();
	
	do
	{
		uint minimum = min( al - am, bl - bm );
		for( uint i = 0; i < minimum; i++ )
		{
			if( a[i+am] > b[i+bm] )
				return true;
			else if( a[i+am] < b[i+bm] )
				return false;
		}
		if( al - am > bl - bm )
			am += minimum;
		else if( al - am < bl - bm )
			bm += minimum;
		else
		{
			am++;
			bm++;
		}
	}while( ( al != am ) && ( bl != bm ) );
	return false;
}
