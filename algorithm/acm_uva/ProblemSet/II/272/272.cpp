#include <iostream>

int main() {
	using namespace std;
	
	char c;
	bool flag = true;
	
	while( cin.get( c ) ) {
		if( c == '\"' ) {
			if( flag ) {
				cout << "``";
			} else {
				cout << "\'\'";
			}
			flag = !flag;
		} else {
			cout.put( c );
		}
	}
	
	return 0;
}
