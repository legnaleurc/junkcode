#include <iostream>
#include <string>
#include <cctype>

typedef unsigned int uint;

int main() {
	using namespace std;
	string tmp;
	
	while( getline( cin, tmp ) ) {
		uint count = 0;
		bool flag = false;
		for( string::const_iterator it = tmp.begin(); it != tmp.end(); ++it ) {
			if( isalpha( *it ) ) {
				flag = true;
			} else if( flag ) {
				flag = false;
				++count;
			}
		}
		cout << count << endl;
	}
	
	return 0;
}
