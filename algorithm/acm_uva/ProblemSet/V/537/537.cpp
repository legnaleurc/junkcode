#include <iostream>
#include <sstream>
#include <string>
using namespace std;

double getValue( string temp );

int main()
{
	int cases, count = 0;
	double value;
	double *P = NULL, *I = NULL, *V = NULL;
	string tmp, temp;
	istringstream iss;
	
	cin >> cases;
	cin.ignore();
	
	while( cases-- )
	{
		getline( cin, tmp );
		iss.str( tmp );
		while( iss >> temp )
		{
			if( temp.find( '=' ) != string::npos )
			{
				value = getValue( temp );
				if( ( temp.find( 'P' ) != string::npos ) && ( temp.find( 'W' ) != string::npos ) )
				{
					P = new double;
					*P = value;
				}
				else if( ( temp.find( 'U' ) != string::npos ) && ( temp.find( 'V' ) != string::npos ) )
				{
					V = new double;
					*V = value;
				}
				else if( ( temp.find( 'I' ) != string::npos ) && ( temp.find( 'A' ) != string::npos ) )
				{
					I = new double;
					*I = value;
				}
			}
		}
		
		cout << "Problem #" << ++count << endl;
		cout.setf( ios::fixed );
		cout.precision( 2 );
		if( P == NULL )
		{
			cout << "P=" << *I * *V << "W" << endl;
		}
		else if( V == NULL )
		{
			cout << "U=" << *P / *I << "V" << endl;
		}
		else if( I == NULL )
		{
			cout << "I=" << *P / *V << "A" << endl;
		}
		cout << endl;
	
		delete P;
		P = NULL;
		delete V;
		V = NULL;
		delete I;
		I = NULL;
		iss.clear();
	}
	
	return 0;
}

double getValue( string temp )
{
	istringstream iss;
	double value;
	
	temp.erase( 0, temp.find( '=' ) + 1 );
	iss.str( temp );
	iss >> value;
	if( temp.find( 'm' ) != string::npos )
	{
		value /= 1000;
	}
	else if( temp.find( 'k' ) != string::npos )
	{
		value *= 1000;
	}
	else if( temp.find( 'M' ) != string::npos )
	{
		value *= 1000000;
	}
	
	return value;
}
