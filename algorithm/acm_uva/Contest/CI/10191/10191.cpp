#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
using namespace std;

int toru( string s );
string torimodosu( int time );

int main()
{
	int num, count = 0, maxd, maxi, temp;
	string hajime, owari, suteru;
	vector<int> start, goal;
	
	while( cin >> num )
	{
		cin.ignore();
		start.push_back( 480 );
		goal.push_back( 0 );
		for( int i = 0; i < num; i++ )
		{
			getline( cin, suteru );
			hajime = suteru.substr( 0, 5 );
			owari = suteru.substr( 6, 5 );
			start.push_back( toru( hajime ) );
			goal.push_back( toru( owari ) );
		}
		sort( start.begin(), start.end() );
		sort( goal.begin(), goal.end() );
		maxd = 0;
		maxi = 0;
		for( int i = 0; i < num + 1; i++ )
		{
			temp = max( maxd, start[i] - goal[i] );
			if( temp > maxd )
			{
				maxd = temp;
				maxi = i;
			}
		}
		
		cout << "Day #" << ++count << ": the longest nap starts at " << torimodosu( goal[maxi] ) << " and will last for ";
		if( maxd < 60 )
		{
			cout << maxd << " minutes." << endl;
		}
		else
		{
			cout << maxd / 60 << " hours and " << maxd % 60 << " minutes." << endl;
		}
		
		start.clear();
		goal.clear();
	}
	return 0;
}

int toru( string s )
{
	istringstream iss;
	int hour, minute;
	
	s.replace( 2, 1, 1, ' ' );
	iss.str( s );
	iss >> hour >> minute;
	
	return ( hour - 10 ) * 60 + minute;
}

string torimodosu( int time )
{
	ostringstream oss;
	int minute = time % 60;
	int hour = ( time - minute ) / 60 + 10;
	
	oss << hour << ':';
	oss.fill( '0' );
	oss.width( 2 );
	oss << minute;
	
	return oss.str();
}
