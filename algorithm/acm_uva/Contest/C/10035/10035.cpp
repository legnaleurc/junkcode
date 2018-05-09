//http://acm.uva.es/p/v100/10035.html
#include <iostream>
#include <string>
using namespace std;

typedef unsigned int uint;

int main()
{
	string a,b;
	uint count;
	
	while( true )
	{
		cin >> a >> b;
		if(a == "0" && b == "0")
			break;
		
		count = 0;
		if( a.length() < b.length() )	//Ensure #a's length being greater than #b's length.
			a.swap( b );
		const uint al = a.length(), bl = b.length();
		uint i, carry = 0;
		for( i = 0; i < bl; i++ )
		{
			if( a[al-1-i] - '0' + b[bl-1-i] - '0' + carry >= 10 )	//If sum >= 10, carry out 1, and carry count plus 1.
			{
				count++;
				carry = 1;
			}
			else
				carry = 0;
		}
		for( ; i < al; i++ )	//Check for remind string.
		{
			if( a[al-1-i] - '0' + carry >= 10 )
			{
				count++;
				carry = 1;
			}
			else
				carry = 0;
		}
		if(count == 1)
			cout << "1 carry operation." << endl;
		else if(count > 1)
			cout << count << " carry operations." << endl;
		else
			cout << "No carry operation." << endl;
	}
	return 0;
}
