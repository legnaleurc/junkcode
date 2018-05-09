#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <cmath>
using namespace std;

const double D = 1.0 / sqrt( 2.0 );

int main()
{
	string temp1,temp2;
	istringstream iss1, iss2;
	int step, count = 0;
	string dir;
	double x = 0.0, y = 0.0, d;
	
	while( getline( cin, temp1 ) )
	{
		if( temp1 == "END" )
		{
			break;
		}
		else
		{
			iss1.str( temp1 );
			while( getline( iss1, temp2, ',' ) )
			{
				if( temp2.find( '.' ) != string::npos )
				{
					temp2.erase( temp2.end()-1 );
				}
			
				iss2.str( temp2 );
				iss2 >> step >> dir;
				iss2.clear();
			
				if( dir == "E" )
				{
					x += step;
				}
				else if( dir == "SE" )
				{
					x += step * D;
					y -= step * D;
				}
				else if( dir == "S" )
				{
					y -= step;
				}
				else if( dir == "SW" )
				{
					x -= step * D;
					y -= step * D;
				}
				else if( dir == "W" )
				{
					x -= step;
				}
				else if( dir == "NW" )
				{
					x -= step * D;
					y += step * D;
				}
				else if( dir == "N" )
				{
					y += step;
				}
				else if( dir == "NE" )
				{
					x += step * D;
					y += step * D;
				}
			}
			d = sqrt( x * x + y * y );
			
			cout << "Map #" << ++count << endl;
			cout.setf( ios::fixed );
			cout.precision( 3 );
			cout << "The treasure is located at (" << x << "," << y << ")." << endl;
			cout << "The distance to the treasure is " << d << "." << endl;
			cout << endl;
			
			x = 0.0;
			y = 0.0;
			iss1.clear();
		}
	}
	
	return 0;
}
