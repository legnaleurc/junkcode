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

/**
 * @brief Constructor. Reads tile from a inputstream \p in.
 * @param in The input stream
 */
Tile::Tile( std::istream & in ): size_( 0 ), top_(), bottom_(), nodes_(), walked_( false ) {
	using namespace std;
	size_t begin = string::npos, end = string::npos;

	// read and normalize upper tile
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

	// read and normalize lower tile
	getline( in, this->bottom_ );
	begin = this->bottom_.find_first_of( "1" );
	end = this->bottom_.find_last_of( "1" );
	if( begin == string::npos || end == string::npos ) {
		this->bottom_.clear();
	} else {
		this->bottom_ = this->bottom_.substr( begin, end + 1 - begin );
	}
	replace( this->bottom_.begin(), this->bottom_.end(), '1', '-' );

	// discard last line
	string line;
	getline( in, line );
}

/**
 * @brief Connect nodes if they can match each other.
 * @param thisIndex The index of this node in table
 * @param that The node to be connect
 * @param thatIndex The index of \p that in table
 */
void Tile::link( size_t thisIndex, Tile & that, size_t thatIndex ) {
	if( this->size_ >= that.size_ && this->top_ == that.bottom_ ) {
		this->nodes_.push_back( thatIndex );
	}
	if( that.size_ >= this->size_ && that.top_ == this->bottom_ ) {
		that.nodes_.push_back( thisIndex );
	}
}

/**
 * @brief Get adjacent nodes.
 * @return A list of adjacent nodes
 */
const std::list< size_t > & Tile::getNodes() const {
	return this->nodes_;
}

/**
 * @brief Mark if this node has been walked.
 * @return true if walked, else false
 */
bool & Tile::walked() {
	return this->walked_;
}

/**
 * @brief DFS implementation.
 * @param v The nodes table
 * @param begin The search start point
 * @param cache The current maximum path cache
 * @param length The current path length
 */
void dfs_( const Vector & v, size_t begin, std::vector< size_t > & cache, size_t length ) {
	using namespace std;

	// if this node has been walked or length is lower than before, skip
	if( v[begin]->walked() || cache[begin] >= length ) {
		return;
	} else {
		// update current high
		cache[begin] = length;
	}

	v[begin]->walked() = true;

	// walk over all adjacent nodes
	for( list< size_t >::const_iterator it = v[begin]->getNodes().begin(); it != v[begin]->getNodes().end(); ++it ) {
		dfs_( v, *it, cache, length + 1 );
	}

	v[begin]->walked() = false;
}

/**
 * @brief DFS shell
 * @param v The nodes table
 * @return The maximum path length
 */
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
			// I use pointer to avoid extra copy
			shared_ptr< Tile > tile( new Tile( cin ) );
			// create adjacent nodes before insert it
			for( size_t i = 0; i < v.size(); ++i ) {
				v[i]->link( i, *tile, v.size() );
			}
			v.push_back( tile );
		}
		cout << dfs( v ) << endl;
	}

	return 0;
}
