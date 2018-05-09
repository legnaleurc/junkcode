#include <iostream>
#include <algorithm>

using namespace std;

const size_t MAX = 1000;

int main() {
	bool prime[MAX];
	fill_n( prime, MAX, true );
	prime[0] = prime[1] = false;
	size_t size = 0;

	for( size_t i = 2; i < MAX; ++i )
		if( prime[i] ) {
			++size;
			for( size_t j = i; j * i < MAX; ++j )
				if( prime[j*i] )
					prime[j*i] = false;
		}

	size_t table[size];
	size_t j = 0;
	for( size_t i = 0; i < MAX; ++i )
		if( prime[i] )
			table[j++] = i;

	size_t * result = new size_t[1000001];
	result[0] = result[1] = 0;
	result[2] = 1;

	for( size_t i = 3; i <= 1000000; ++i ) {
		size_t factor = 0;
		size_t temp = i;
		j = 0;
		while( temp != 1 && j < size ) {
			if( temp % table[j] == 0 ) {
				++factor;
				temp /= table[j];
			} else {
				++j;
			}
		}
		if( temp != 1 ) {
			++factor;
		}
		result[i] = result[i-1] + factor;
	}

	size_t x;

	while( cin >> x ) {
		cout << result[x] << endl;
	}

	delete [] result;

	return 0;
}
