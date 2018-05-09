#include <iostream>
#include <string>
using namespace std;

int main()
{
	int cases;
	string input;

	cin >> cases;
	cin.ignore();
	while( cases-- )
	{
		getline( cin, input );
		if( input == "1" || input == "4" || input == "78" )
		{
			cout << "+" << endl;
		}
		else if( input.substr( input.length() - 2, 2 ) == "35" )
		{
			cout << "-" << endl;
		}
		else if( *( input.begin() ) == '9' && *( input.end() - 1 ) == '4' )
		{
			cout << "*" << endl;
		}
		else if( input.substr( 0, 3 ) == "190" )
		{
			cout << "?" << endl;
		}
	}

	return 0;
}