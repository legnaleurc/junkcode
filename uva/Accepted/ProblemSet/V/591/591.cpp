#include <iostream>
using namespace std;

typedef unsigned int uint;

struct Array
{
	uint data[50];
	uint sum;
	uint avg;
	uint size;
};

int main()
{
	Array walls;
	uint count = 0;
	
	while( true )
	{
		cin >> walls.size;
		if( walls.size == 0 )
			break;
					
		walls.sum = 0;
		for( uint i = 0; i < walls.size; i++ )
		{
			cin >> walls.data[i];
			walls.sum += walls.data[i];
		}
		walls.avg = walls.sum / walls.size;
		uint result = 0;
		for( uint i = 0; i < walls.size; i++ )
		{
			if( walls.data[i] > walls.avg )
				result += walls.data[i] - walls.avg;
		}
		
		cout << "Set #" << ++count << endl;
		cout << "The minimum number of moves is " << result << "." << endl;
		cout << endl;
	}
	
	return 0;
}
