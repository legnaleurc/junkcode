#include <iostream>
#include <algorithm>
#include <string>
using namespace std;

typedef unsigned int uint;

uint revadd( string & s ) {
	uint counter = 0;
	string clone( s );

	reverse( clone.begin(), clone.end() );

	while( clone != s ) {
		++counter;
		uint carry = 0;
		for( int i = s.length() - 1; i >= 0; --i ) {
			s[i] += clone[i] - '0' + carry;
			if( s[i] > '9' ) {
				s[i] -= 10;
				carry = 1;
			} else {
				carry = 0;
			}
		}
		if( carry > 0 ) {
			s = "1" + s;
		}
		clone = s;
		reverse( clone.begin(), clone.end() );
	}
	return counter;
}

int main() {
	uint cases;
	string tmp;

	cin >> cases;
	while( cases-- ) {
		cin >> tmp;

		cout << revadd( tmp ) << " " << tmp << endl;
	}

	return 0;
}
