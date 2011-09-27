#include <iostream>
#include <algorithm>
#include <vector>
#include <memory>
#include <list>

class Tile {
public:
	explicit Tile( std::istream & in );
	void link( size_t thisIndex, Tile & that, size_t thatIndex );
	const std::list< size_t > & getNodes() const;
	bool & walked();
private:
	std::size_t size_;
	std::string top_;
	std::string bottom_;
	std::list< size_t > nodes_;
	bool walked_;
};

typedef std::vector< std::shared_ptr< Tile > > Vector;

Tile::Tile( std::istream & in ): size_( 0 ), top_(), bottom_(), nodes_(), walked_( false ) {
	using namespace std;
	size_t begin = string::npos, end = string::npos;

	getline( in, this->top_ );
	this->size_ = this->top_.size();
	begin = this->top_.find_first_of( "0" );
	end = this->top_.find_last_of( "0" );
	if( begin == string::npos || end == string::npos ) {
		this->top_.clear();
	} else {
		this->top_ = this->top_.substr( begin, end + 1 - begin );
	}
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

void Tile::link( size_t thisIndex, Tile & that, size_t thatIndex ) {
	if( this->size_ >= that.size_ && this->top_ == that.bottom_ ) {
		this->nodes_.push_back( thatIndex );
	}
	if( that.size_ >= this->size_ && that.top_ == this->bottom_ ) {
		that.nodes_.push_back( thisIndex );
	}
}

const std::list< size_t > & Tile::getNodes() const {
	return this->nodes_;
}

bool & Tile::walked() {
	return this->walked_;
}

void dfs_( const Vector & v, size_t begin, std::vector< size_t > & cache, size_t length ) {
	using namespace std;

	if( v[begin]->walked() || cache[begin] >= length ) {
		return;
	} else {
		cache[begin] = length;
	}

	v[begin]->walked() = true;

	for_each( v[begin]->getNodes().begin(), v[begin]->getNodes().end(), [&]( size_t node ) {
		dfs_( v, node, cache, length + 1 );
	} );

	v[begin]->walked() = false;
}

std::size_t dfs( const Vector & v ) {
	using namespace std;

	vector< size_t > cache( v.size() );
	fill( cache.begin(), cache.end(), 0 );

	dfs_( v, 0, cache, 1 );

	return *max_element( cache.begin(), cache.end() );
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
			shared_ptr< Tile > tile( new Tile( cin ) );
			for( size_t i = 0; i < v.size(); ++i ) {
				v[i]->link( i, *tile, v.size() );
			}
			v.push_back( tile );
		}
		cout << dfs( v ) << endl;
	}

	return 0;
}
