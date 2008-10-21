#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

int main()
{
	int round;
	unsigned int wrong;
	string word, guess;
	string::iterator si;
	size_t seed;

	while( cin >> round )
	{
		if( round < 0 )
			break;

		cin.ignore();
		getline( cin, word );
		getline( cin, guess );

		sort( word.begin(), word.end() );
		si = unique( word.begin(), word.end() );
		word.resize( si - word.begin() );
		sort( guess.begin(), guess.begin() );
		si = unique( guess.begin(), guess.end() );
		guess.resize( si - guess.begin() );

		wrong = 0;
		for( unsigned int i = 0; i < guess.length(); i++ )
		{
			seed = word.find( guess[i] );
			if( seed == string::npos )
			{
				wrong++;
			}
			else
			{
				word.erase( seed, 1 );
			}
		}

		cout << "Round " << round << endl;
		if( wrong >= 7 )
		{
			cout << "You lose." << endl;
		}
		else if( word.length() != 0 )
		{
			cout << "You chickened out." << endl;
		}
		else
		{
			cout << "You win." << endl;
		}
	}

	return 0;
}
