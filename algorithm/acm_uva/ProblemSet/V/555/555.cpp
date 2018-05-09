#include <iostream>
#include <algorithm>
#include <cctype>
using namespace std;

struct Hand
{
	char suit;
	char rank;
};

bool gt( Hand a, Hand b );

class Player
{
public:
	Player();
	void sort();
	Hand& operator []( unsigned int index );
private:
	Hand cards[13];
};

Player::Player()
{
	for( int i = 0; i < 13; i++ )
	{
		cards[i].rank = 0;
		cards[i].suit = 0;
	}
}

void Player::sort()
{
	std::sort( cards, cards + 13, gt );
}

Hand& Player::operator []( unsigned int index )
{
	return cards[index];
}

istream& operator >>( istream& Input, Hand& Object )
{
	Input >> Object.suit >> Object.rank;

	return Input;
}

bool gt( Hand a, Hand b )
{
	if( isdigit( a.rank ) )
		a.rank -= 48;
	switch( a.rank )
	{
	case 'T':
		a.rank = 10;
		break;
	case 'J':
		a.rank = 11;
		break;
	case 'Q':
		a.rank = 12;
		break;
	case 'K':
		a.rank = 13;
		break;
	case 'A':
		a.rank = 14;
		break;
	}
	switch( a.suit )
	{
	case 'C':
		a.suit = 15;
		break;
	case 'D':
		a.suit = 30;
		break;
	case 'S':
		a.suit = 45;
		break;
	case 'H':
		a.suit = 60;
		break;
	}
	if( isdigit( b.rank ) )
		b.rank -= 48;
	switch( b.rank )
	{
	case 'T':
		b.rank = 10;
		break;
	case 'J':
		b.rank = 11;
		break;
	case 'Q':
		b.rank = 12;
		break;
	case 'K':
		b.rank = 13;
		break;
	case 'A':
		b.rank = 14;
		break;
	}
	switch( b.suit )
	{
	case 'C':
		b.suit = 15;
		break;
	case 'D':
		b.suit = 30;
		break;
	case 'S':
		b.suit = 45;
		break;
	case 'H':
		b.suit = 60;
		break;
	}

	return a.rank+a.suit < b.rank+b.suit;
}

int main()
{
	char dealer;
	Player P1,P2,P3,P4;
	int i;

	while( cin >> dealer )
	{
		if( dealer == '#' )
			break;
		
		switch( dealer )
		{
		case 'N':
			for( i = 0; i < 13; i++ )
			{
				cin >> P4[i] >> P1[i] >> P2[i] >> P3[i];
			}
			break;
		case 'E':
			for( i = 0; i < 13; i++ )
			{
				cin >> P1[i] >> P2[i] >> P3[i] >> P4[i];
			}
			break;
		case 'W':
			for( i = 0; i < 13; i++ )
			{
				cin >> P3[i] >> P4[i] >> P1[i] >> P2[i];
			}
			break;
		case 'S':
			for( i = 0; i < 13; i++ )
			{
				cin >> P2[i] >> P3[i] >> P4[i] >> P1[i];
			}
			break;
		}
		P1.sort();
		P2.sort();
		P3.sort();
		P4.sort();

		cout << "S:";
		for( i = 0; i < 13; i++ )
		{
			cout << " " << P1[i].suit << P1[i].rank;
		}
		cout << endl;
		cout << "W:";
		for( i = 0; i < 13; i++ )
		{
			cout << " " << P2[i].suit << P2[i].rank;
		}
		cout << endl;
		cout << "N:";
		for( i = 0; i < 13; i++ )
		{
			cout << " " << P3[i].suit << P3[i].rank;
		}
		cout << endl;
		cout << "E:";
		for( i = 0; i < 13; i++ )
		{
			cout << " " << P4[i].suit << P4[i].rank;
		}
		cout << endl;
	}

	return 0;
}
