#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
using namespace std;

typedef unsigned int uint;

struct Array
{
	uint data[30];
	uint size;
};

int main()
{
	string temp;
	Array pancake;
	uint *maximum;
	
	while( getline( cin, temp ) )
	{
		istringstream iss( temp );
		
		pancake.size = 0;
		while( iss >> pancake.data[pancake.size] )
		{
			pancake.size++;
		}
		cout << temp << endl;
		
		for( uint i = pancake.size; i > 0; i-- )
		{
			maximum = max_element( pancake.data, pancake.data + i );
			if( maximum != pancake.data + i - 1 )
			{
				if( maximum != pancake.data )
				{
					reverse( pancake.data, maximum + 1 );
					cout << static_cast<uint>( pancake.data + pancake.size - maximum ) << " ";
				}
				reverse( pancake.data, pancake.data + i );
				cout << pancake.size - i + 1 << " ";
			}
		}
		cout << 0 << endl;
	}
	
	return 0;
}
