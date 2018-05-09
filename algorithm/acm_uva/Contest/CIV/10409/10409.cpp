#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

class Dice
{
	public:
		Dice();
		void goEast();
		void goSouth();
		void goWest();
		void goNorth();
		int getUp();
		void clear();
	private:
		int up;
		int north;
		int west;
		int east;
		int south;
		int down;
};

Dice::Dice()
{
	up = 1;
	north = 2;
	west = 3;
	east = 4;
	south = 5;
	down = 6;
}

void Dice::goEast()
{
	swap( west, down );
	swap( down, east );
	swap( east, up );
}

void Dice::goSouth()
{
	swap( up, north );
	swap( north, down );
	swap( down, south );
}

void Dice::goWest()
{
	swap( up, east );
	swap( east, down );
	swap( down, west );
}

void Dice::goNorth()
{
	swap( south, down );
	swap( down, north );
	swap( north, up );
}

int Dice::getUp()
{
	return up;
}

void Dice::clear()
{
	up = 1;
	north = 2;
	west = 3;
	east = 4;
	south = 5;
	down = 6;
}

int main()
{
	int step;
	string dir;
	Dice dice;
	
	while( cin >> step )
	{
		cin.ignore();
		if( step == 0 )
		{
			break;
		}
		else
		{
			while( step-- )
			{
				getline( cin, dir );
				if( dir == "east" )
				{
					dice.goEast();
				}
				else if( dir == "south" )
				{
					dice.goSouth();
				}
				else if( dir == "west" )
				{
					dice.goWest();
				}
				else if( dir == "north" )
				{
					dice.goNorth();
				}
			}
			cout << dice.getUp() << endl;
			dice.clear();
		}
	}
	
	return 0;
}
