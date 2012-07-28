#include <vector>
#include <ostream>

namespace pack {

namespace detail {

class PackBase {
public:
	long getScore() const;

	bool operator <( const PackBase & that ) const;
	bool operator ==( const PackBase & that ) const;
	bool operator >( const PackBase & that ) const;
	bool operator <=( const PackBase & that ) const;
	bool operator >=( const PackBase & that ) const;
	bool operator !=( const PackBase & that ) const;

protected:
	PackBase();
	PackBase( const PackBase & that );
	PackBase( PackBase && that );
	PackBase & operator =( const PackBase & that );
	PackBase & operator =( PackBase && that );
	virtual ~PackBase();

	long & score();

private:
	long score_;
};

}

template< typename T >
class Pack: public detail::PackBase {
public:
	Pack(): detail::PackBase(), items_() {
	}

	const std::vector< T > & getItems() const {
		return this->items_;
	}

	Pack add( const T & key, long value ) const {
		Pack tmp( *this );
		tmp.score() += value;
		tmp.items_.push_back( key );
		return std::move( tmp );
	}

private:
	std::vector< T > items_;
};

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

}
