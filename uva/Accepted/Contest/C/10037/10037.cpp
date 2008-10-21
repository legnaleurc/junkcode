//http://acm.uva.es/p/v100/10037.html
#include <iostream>
#include <algorithm>
#include <sstream>
using namespace std;

typedef unsigned int uint;

struct Array
{
	uint *content;
	size_t size;
};

int main()
{
	uint cases, temp;	//#temp is used to store total time.
	Array start;	//Stores all people's time.
	ostringstream oss;	//Result stringstream.
	
	cin >> cases;
	while( cases-- )
	{
		cin >> start.size;
		start.content = new uint[start.size];
		for( uint i = 0; i < start.size; i++ )
		{
			cin >> start.content[i];
		}
		sort( start.content, start.content + start.size );
		
		temp = 0;
		while( start.size >= 4 )	//Case of people >= 4.
		{
			if( ( 2 * start.content[1] ) < ( start.content[0] + start.content[ start.size - 2 ]) )	//Algorithm 1
			{
				oss << start.content[0] << " " << start.content[1] << endl;
				temp += start.content[1];
				oss << start.content[0] << endl;
				temp += start.content[0];
				oss << start.content[ start.size - 2 ] << " " << start.content[ start.size - 1 ] << endl;
				temp += start.content[ start.size - 1 ];
				start.size -= 2;
				oss << start.content[1] << endl;
				temp += start.content[1];
			}
			else	//Algorithm 2
			{
				oss << start.content[0] << " " << start.content[ start.size - 1 ] << endl;
				temp += start.content[ start.size - 1 ];
				start.size--;
				oss << start.content[0] << endl;
				temp += start.content[0];
			}
		}
		if( start.size > 1 )
		{
			if( start.size == 3 )
			{
				oss << start.content[0] << " " << start.content[2] << endl;
				temp += start.content[2];
				oss << start.content[0] << endl;
				temp += start.content[0];
			}
			oss << start.content[0] << " " << start.content[1] << endl;
			temp += start.content[1];
		}
		else
		{
			oss << start.content[0] << endl;
			temp += start.content[0];
		}
		
		cout << temp << endl;
		cout << oss.str();
		if( cases > 0 )
			cout << endl;
		
		oss.clear();
		oss.str( "" );
		delete[] start.content;
	}
	
	return 0;
}

/*
過橋走法有兩種，設A,B為最快兩人，Y,Z為最慢兩人，則
第一種：
A B
A
Y Z
B
A B
耗時：A + 3B + Z
第二種：
A Z
A
A Y
A
A B
耗時：2A + B + Y + Z
故每次都必須判斷哪種方法的時間較短，以採取最佳方法。（相消為2B : A + Y）
以上走法只適合四人以上，三人以下時再另外處理。
*/
