#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
using namespace std;

int main()
{
	string input, temp;
	istringstream iss;
	
	while( getline( cin, input ) )
	{
		iss.str( input );
		while( iss >> temp )
		{
			reverse( temp.begin(), temp.end() );
			cout << temp;
			if( iss.eof() )
			{
				cout << endl;
			}
			else
			{
				cout << " ";
			}
		}
		iss.clear();
	}
	
	return 0;
}
