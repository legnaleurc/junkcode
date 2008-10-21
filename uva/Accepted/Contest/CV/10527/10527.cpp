//http://acm.uva.es/p/v105/10527.html
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdlib>
using namespace std;

int main()
{
	string input, temp, posibility;
	vector<int> v;
	int mod, buffer;

	while( getline( cin, input ) )
	{
		if( input == "-1" )
			break;

		if( input.length() == 1 )
		{
			cout << "1" << input << endl;
		}
		else
		{
			for( int j = 9; j > 1; j-- )
			{
				if( input.length() == 1 )
				{
					v.push_back( input[0] - 48 );
					break;
				}
				temp = input;
				for( unsigned int i = 0; i < input.length() - 1; i++ )
				{
					buffer = atoi( temp.substr( i, 2 ).c_str() ) / j;
					if( buffer >= 10 )
					{
						posibility.push_back( ( buffer / 10 ) + 48 );
						posibility.push_back( ( buffer % 10 ) + 48 );
					}
					else
					{
						posibility.push_back( buffer + 48 );
					}
					mod = atoi( temp.substr( i, 2 ).c_str() ) % j;
					temp[i+1] = mod + 48;
				}
				if( mod == 0 )
				{
					v.push_back( j );
					j++;
					input = posibility;
				}
				posibility = "";
			}

			if( input.length() > 1 )
			{
				cout << "There is no such number.";
			}
			else
			{
				sort( v.begin(), v.end() );
				for( unsigned int i = 0; i < v.size(); i++ )
				{
					cout << v[i];
				}
			}
			cout << endl;

			v.clear();
		}
	}

	return 0;
}