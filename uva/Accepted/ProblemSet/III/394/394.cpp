#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>

typedef std::pair< int, int > Bound;

struct Array {
	Array( int base_ = 0, int size_ = 0 ) : base( base_ ), size( size_ ) {}
	int base;
	int size;
	std::vector< Bound > bound;
	
	void fetch() {
		cache.resize( bound.size() );
		cd( 0 );
		c0 = base;
		for( int i = 0; i < bound.size(); ++i ) {
			c0 -= cache[i] * bound[i].first;
		}
	}
	
	int run( const std::vector< int > & index ) {
		int sum = c0;
		for( int i = 0; i < bound.size(); ++i ) {
			sum += cache[i] * index[i];
		}
		
		return sum;
	}
	
private:
	std::vector< int > cache;
	int c0;
	
	void cd( int d ) {
		if( d == bound.size() - 1 ) {
			cache[d] = size;
		} else {
			cd( d + 1 );
			cache[d] = cache[d+1] * ( bound[d+1].second - bound[d+1].first + 1 );
		}
	}
};

int main() {
	using namespace std;
	
	int n, r;
	
	cin >> n >> r;
	cin.ignore();
	
	std::map< string, Array > data;
	
	while( n-- ) {
		// read array define
		string line, key;
		int base, size, dim, l, u;
		
		getline( cin, line );
		
		istringstream sin( line );
		
		sin >> key >> base >> size >> dim;
		
		data[key] = Array( base, size );
		while( dim-- ) {
			sin >> l >> u;
			data[key].bound.push_back( Bound( l, u ) );
		}
		data[key].fetch();
	}
	
	std::vector< int > index;
	while( r-- ) {
		// read array reference
		string line, key;
		
		getline( cin, line );
		
		istringstream sin( line );
		
		sin >> key;
		
		size_t tmp = data[key].bound.size();
		int i;
		while( tmp-- ) {
			sin >> i;
			index.push_back( i );
		}
		
		cout << key << '[' << index[0];
		for( i = 1; i < index.size(); ++i ) {
			cout << ", " << index[i];
		}
		cout << "] = " << data[key].run( index ) << endl;
		
		index.clear();
	}
	
	return 0;
}
