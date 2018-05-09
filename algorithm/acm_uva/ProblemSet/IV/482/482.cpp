#include <iostream>
#include <vector>
#include <string>
#include <sstream>
using namespace std;

int main()
{
	int cases;
	vector<string> v;
	string index, content;
	istringstream issi, issc;
	int i;
	string c;
	
	cin >> cases;
	cin.ignore();
	while(cases--)
	{
		cin.ignore();
		getline( cin, index );
		getline( cin, content );
		issi.str( index );
		issc.str( content );
		while( issi >> i && issc >> c )
		{
			if( v.size() <= i )
				v.resize( i );
			v[i-1] = c;
		}
		
		for( int i = 0; i < v.size(); i++ )
		{
			cout << v[i] << endl;
		}
		if( cases > 0 )
			cout << endl;
		
		v.clear();
		issi.clear();
		issi.str( "" );
		issc.clear();
		issc.str( "" );
	}
	
	return 0;
}
