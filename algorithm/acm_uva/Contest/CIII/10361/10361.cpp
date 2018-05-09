#include <iostream>
#include <sstream>
#include <string>
using namespace std;

int main()
{
	int cases, count;
	string l1, l2, s[5], temp;
	ostringstream oss;
	size_t token1, token2, token3, token4;
	
	cin >> cases;
	cin.ignore();
	
	while( cases-- )
	{
		getline( cin, l1 );
		getline( cin, l2 );
		
		count = 0;
		for( int i = 0; i < l1.length(); i++ )
		{
			if( l1[i] == '<' || l1[i] == '>' )
			{
				count++;
			}
			else
			{
				s[count].push_back( l1[i] );
			}
		}
		
		oss << s[3] << s[2] << s[1] << s[4];
		temp = oss.str();
		
		l2.replace( l2.find( "..." ) , 3, temp );
		
		for( int i = 0; i < 5; i++ )
		{
			cout << s[i];
			s[i] = "";
		}
		cout << endl;
		cout << l2 << endl;
		
		temp = "";
		oss.clear();
		oss.str( "" );
	}
	
	return 0;
}
