#include <iostream>
#include <algorithm>
#include <vector>
#include <memory>
#include <set>
#include <queue>
#include <list>

class Tile {
public:
	explicit Tile( std::istream & in );
	void link( Tile & that );
	const std::list< Tile * > & getNodes() const;
	void debug() const;
private:
	std::size_t size_;
	std::string top_;
	std::string bottom_;
	std::list< Tile * > nodes_;
};

typedef std::vector< std::shared_ptr< Tile > > Vector;

Tile::Tile( std::istream & in ): size_( 0 ), top_(), bottom_(), nodes_() {
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

void Tile::link( Tile & that ) {
	if( this->size_ >= that.size_ && this->top_ == that.bottom_ ) {
		this->nodes_.push_back( &that );
	}
	if( that.size_ >= this->size_ && that.top_ == this->bottom_ ) {
		that.nodes_.push_back( this );
	}
}

const std::list< Tile * > & Tile::getNodes() const {
	return this->nodes_;
}

void Tile::debug() const {
	using namespace std;
	cout << this->top_ << endl;
	cout << this->bottom_ << endl;
	cout << this->size_ << endl;
}

std::size_t bfs( const Vector & v ) {
	using namespace std;

	size_t maxlen = 0;
	queue< pair< Tile *, set< Tile * > > > q;
	q.push( make_pair( v[0].get(), set< Tile * >() ) );
	q.back().second.insert( v[0].get() );

	while( !q.empty() ) {
		pair< Tile *, set< Tile * > > path( q.front() );
		q.pop();
		int counter = 0;
		for_each( path.first->getNodes().begin(), path.first->getNodes().end(), [&]( Tile * const node ) {
			if( path.second.find( node ) == path.second.end() ) {
				q.push( make_pair( node, path.second ) );
				q.back().second.insert( node );
				++counter;
			}
		} );
		if( counter <= 0 && maxlen < path.second.size() ) {
			maxlen = path.second.size();
		}
	}

	return maxlen;
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
			for_each( v.begin(), v.end(), [tile]( shared_ptr< Tile > leaf ) {
				leaf->link( *tile );
			} );
			v.push_back( tile );
		}
		cout << bfs( v ) << endl;
	}

	return 0;
}
