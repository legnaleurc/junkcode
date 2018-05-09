#include <iostream>
#include <string>

using std::cin;
using std::cout;
using std::string;
using std::ios;

const double table[5][3] = { { 0.10, 0.06, 0.02 }, { 0.25, 0.15, 0.05 }, { 0.53, 0.33, 0.13 }, { 0.87, 0.47, 0.17 }, { 1.44, 0.80, 0.30 } };
const int DAY = 0;
const int EVENING = 600;
const int NIGHT = 840;
const int WHOLE = 1440;

int main()
{
	char distance;
	string phone;
	int begin_t[2], end_t[2];
	int begin, end, day, evening, night;
	
	while( cin >> distance )
	{
		if( distance == '#' )
			break;
		cin >> phone >> begin_t[0] >> begin_t[1] >> end_t[0] >> end_t[1];
		begin = ( begin_t[0] < 8 ) ? ( ( begin_t[0] + 16 ) * 60 + begin_t[1] ) : ( ( begin_t[0] - 8 ) * 60 + begin_t[1] );
		end = ( end_t[0] < 8 ) ? ( ( end_t[0] + 16 ) * 60 + end_t[1] ) : ( ( end_t[0] - 8 ) * 60 + end_t[1] );
		if( end <= begin )
			end += WHOLE;
		
		day = evening = night = 0;
		while( begin != end )
		{
			if( begin >= DAY && begin < EVENING )
			{
				if( end < EVENING )
				{
					day += end - begin;
					begin = end;
				}
				else
				{
					day += EVENING - begin;
					begin = EVENING;
				}
			}
			else if( begin >= EVENING && begin < NIGHT )
			{
				if( end < NIGHT )
				{
					evening += end - begin;
					begin = end;
				}
				else
				{
					evening += NIGHT - begin;
					begin = NIGHT;
				}
			}
			else if( begin >= NIGHT && begin < WHOLE )
			{
				if( end < WHOLE )
				{
					night += end - begin;
					begin = end;
				}
				else
				{
					night += WHOLE - begin;
					begin = DAY;
					end -= WHOLE;
				}
			}
		}
		
		cout << "  " << phone;
		cout.width( 6 );
		cout.setf( ios::right );
		cout << day;
		cout.width( 6 );
		cout.setf( ios::right );
		cout << evening;
		cout.width( 6 );
		cout.setf( ios::right );
		cout << night;
		cout << "  " << distance;
		cout.width( 8 );
		cout.setf( ios::right );
		cout.setf( ios::fixed );
		cout.precision( 2 );
		cout << ( day * table[distance-'A'][0] + evening * table[distance-'A'][1] + night * table[distance-'A'][2] );
		cout << '\n';
	}
	return 0;
}
