#include <iostream>
using namespace std;

typedef unsigned int uint;

struct Array
{
	uint data[10000];
	uint size;
};

void insert( uint t, Array& num, uint index );

int main()
{
	Array num;
	uint temp;
	bool flag;
	
	num.size = 0;
	
	cin >> num.data[num.size++];
	cout << num.data[0] << endl;
	while( cin >> temp )
	{
		flag = true;
		for( uint i = 0; i < num.size; i++ )
			if( temp <= num.data[i] )
			{
				insert( temp, num, i );
				flag = false;
				break;
			}
		if( flag )
			insert( temp, num, num.size );
		
		if( num.size % 2 == 1 )
			cout << num.data[num.size / 2] << '\n';
		else
			cout << ( num.data[num.size / 2] + num.data[num.size / 2 - 1] ) / 2 << '\n';
	}
	
	return 0;
}

void insert( uint t, Array& num, uint index )
{
	uint i = num.size++;
	
	while( i-- > index )
	{
		num.data[i] = num.data[i-1];
	}
	num.data[index] = t;
}
