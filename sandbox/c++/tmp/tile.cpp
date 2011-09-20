#include <iostream>
#include <algorithm>
#include <vector>

class Tile {
public:
	explicit Tile( std::istream & in );
	int match( const Tile & that ) const;
	void debug() const;
private:
	std::size_t size_;
	std::string top_;
	std::string bottom_;
};

typedef std::vector< Tile > Vector;

Tile::Tile( std::istream & in ): size_( 0 ), top_(), bottom_() {
	using namespace std;
	size_t begin = string::npos, end = string::npos;

	getline( in, this->top_ );
	this->size_ = this->top_.size();
	begin = this->top_.find_first_of( "0" );
	end = this->top_.find_last_of( "0" );
	this->top_ = this->top_.substr( begin, end + 1 - begin );
	replace( this->top_.begin(), this->top_.end(), '0', '-' );

	getline( in, this->bottom_ );
	begin = this->bottom_.find_first_of( "1" );
	end = this->bottom_.find_last_of( "1" );
	if( begin == string::npos || end == string::npos ) {
		this->bottom_.clear();
	} else {
		this->bottom_ = this->bottom_.substr( begin, end + 1 - begin );
	}
	replace( this->bottom_.begin(), this->bottom_.end(), '1', '-' );

	string line;
	getline( in, line );
}

int Tile::match( const Tile & that ) const {
	if( this->size_ >= that.size_ && this->top_ == that.bottom_ ) {
		return 1;
	}
	return 0;
}

void Tile::debug() const {
	using namespace std;
	cout << this->top_ << endl;
	cout << this->bottom_ << endl;
	cout << this->size_ << endl;
}

int dag( const Vector & v ) {
	using namespace std;
	vector< int > lengthTo( v.size() );
	fill( lengthTo.begin(), lengthTo.end(), 1 );
	for( int row = 0; row < v.size(); ++row ) {
		for( int col = 1; col < v.size(); ++col ) {
//	for( int row = 0; row < v.size() - 1; ++row ) {
//		for( int col = row + 1; col < v.size(); ++col ) {
			int m = v[row].match( v[col] );
			if( m > 0 && lengthTo[col] < lengthTo[row] + m ) {
				lengthTo[col] = lengthTo[row] + m;
			}
		}
	}
 	for( int i = 0; i < lengthTo.size(); ++i ) {
 		cout << "> " << i << endl;
 	}
	return *max_element( lengthTo.begin(), lengthTo.end() );
}

int main() {
	using namespace std;

	int n, m;
	cin >> n;
	while( n-- ) {
		cin >> m;
		cin.ignore();
		Vector v;
		while( m-- ) {
			v.push_back( Tile( cin ) );
		}
		cout << dag( v ) << endl;
	}

	return 0;
}
