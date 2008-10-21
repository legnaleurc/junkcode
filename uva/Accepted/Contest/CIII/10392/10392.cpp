#include <iostream>
#include <vector>
#include <algorithm>
using std::cin;
using std::cout;
using std::endl;
using std::vector;
using std::fill_n;

typedef long long int llint;

template <typename T>
struct Array {
	Array( size_t _size ) {
		array = new T[_size];
		size = _size;
	}
	~Array() {
		delete[] array;
	}
	T * array;
	size_t size;
};

int main()
{
	llint input, temp;

	Array<bool> prime( 40000 );
	fill_n( prime.array, 40000, true );
	prime.array[0] = prime.array[1] = false;
	for( size_t i = 2; i * i < 40000; ++i )
		if( prime.array[i] )
			for( size_t j = i; j * i < 40000; ++j )
				if( prime.array[j*i] )
					prime.array[j*i] = false;

	vector<llint> table;
	for( size_t i = 0; i < prime.size; ++i )
		if( prime.array[i] )
			table.push_back( i );
	
	while( cin >> input )
	{
		if( input == -1 )
			break;
		
		temp = input;
		size_t i;
		for( i = 0; table[i] * table[i] <= input && i < table.size() ; ++i )
		{
			while( temp % table[i] == 0 )
			{
				cout << "    " << table[i] << endl;
				temp /= table[i];
			}
		}
		if( temp != 1 )
		{
			cout << "    " << temp << endl;
		}
		cout << endl;
	}
	
	return 0;
}
