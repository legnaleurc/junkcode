#include <iostream>
#include <algorithm>
#include <iterator>
#include <map>
#include <vector>
#include <cassert>

template< typename T >
class Pack {
public:
	Pack(): score_( 0L ), items_() {
	}

	long getScore() const {
		return this->score_;
	}
	const std::vector< T > & getItems() const {
		return this->items_;
	}

	Pack add( const T & key, long value ) const {
		Pack tmp( *this );
		tmp.score_ += value;
		tmp.items_.push_back( key );
		return tmp;
	}

	bool operator <( const Pack & that ) const {
		if( this->score_ < that.score_ ) {
			return true;
		}
		return false;
	}
	bool operator ==( const Pack & that ) const {
		if( this->score_ == that.score_ ) {
			return true;
		}
		return false;
	}
	bool operator >( const Pack & that ) const {
		return !( ( *this ) < that || ( *this ) == that );
	}
	bool operator <=( const Pack & that ) const {
		return ( *this ) < that && ( *this ) == that;
	}
	bool operator >=( const Pack & that ) const {
		return !( ( *this ) < that );
	}
	bool operator !=( const Pack & that ) const {
		return !( ( *this ) == that );
	}

private:
	long score_;
	std::vector< T > items_;
};

#ifndef NDEBUG
template < typename T >
std::ostream & operator <<( std::ostream & out, const Pack< T > & p ) {
	out << "( " << p.getScore() << ", ";
	if( p.getItems().empty() ) {
		out << "[]";
	} else {
		out << "[ " << p.getItems()[0];
		for( std::size_t i = 1; i < p.getItems().size(); ++i ) {
			out << ", " << p.getItems()[i];
		}
		out << " ]";
	}
	out << " )";
	return out;
}
#endif

template< typename T >
class DepthFirstSearch {
public:
	DepthFirstSearch( long limit, const std::map< T, long > & items ):
	keys_(),
	values_(),
	limit_( limit ) {
		typedef std::pair< T, long > Pair;
		typedef std::vector< Pair > PairList;
		PairList pl;
		for_each( items.begin(), items.end(), [&pl]( const Pair & p )->void {
			pl.push_back( p );
		} );
		sort( pl.begin(), pl.end() );
		for_each( pl.begin(), pl.end(), [this]( const Pair & p )->void {
			this->keys_.push_back( p.first );
			this->values_.push_back( p.second );
		} );
	}

	Pack< T > operator ()() const {
		return ( *this )( 0, Pack< T >() );
	}

	Pack< T > operator ()( std::size_t n, const Pack< T > & p ) const {
		if( p.getScore() > this->limit_ ) {
			return Pack< T >();
		} else if( n == this->keys_.size() ) {
			return p;
		} else {
			auto a = ( *this )( n + 1, p );
			auto b = ( *this )( n + 1, p.add( this->keys_[n], this->values_[n] ) );
			return std::max( a, b );
		}
	}

private:
	std::vector< T > keys_;
	std::vector< long > values_;
	long limit_;
};

int main() {
	int m;
	std::cin >> m;
	while( m-- ) {
		int n;
		long limit;
		std::cin >> limit >> n;
		std::map< int, long > items;
		for( int i = 0; i < n; ++i ) {
			long tmp;
			std::cin >> tmp;
			items.insert( std::make_pair( i, tmp ) );
		}
		std::vector< long > results;
		while( !items.empty() ) {
			Pack< int > p = DepthFirstSearch< int >( limit, items )();
			assert( !p.getItems().empty() || !"picked nothing" );
			results.push_back( p.getScore() );
			for_each( p.getItems().begin(), p.getItems().end(), [&items]( int i )->void {
				auto it = items.find( i );
				items.erase( it );
			} );
		}
		std::cout << results[0];
		for( std::size_t i = 1; i < results.size(); ++i ) {
			std::cout << ' ' << results[i];
		}
		std::cout << std::endl;
	}
	return 0;
}
