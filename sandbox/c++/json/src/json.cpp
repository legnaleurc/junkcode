#include "json.hpp"

#include <boost/any.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/std_pair.hpp>

namespace {

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

struct nullptr_t_ : qi::symbols< char, void * > {
	nullptr_t_() {
		add( "null", nullptr );
	}
} nullptr_;

typedef std::map< std::string, boost::any > Object_;
typedef std::vector< boost::any > Array_;

template< typename Iterator >
struct Grammar : qi::grammar< Iterator, boost::any(), ascii::space_type > {
	Grammar(): Grammar::base_type( start ) {
		using qi::lexeme;
		using qi::double_;
		using qi::bool_;
		using ascii::char_;

		start = value_rule.alias();
		object_rule = '{' >> pair_rule % ',' >> '}';
		pair_rule = string_rule >> ':' >> value_rule;
		value_rule = object_rule | array_rule | string_rule | nullptr_ | double_ | bool_;
		array_rule = '[' >> value_rule % ',' >> ']';
		string_rule = lexeme[ '\"' >> *( char_ - '\"' ) >> '\"' ];
	}

	qi::rule< Iterator, boost::any(), ascii::space_type > start;
	qi::rule< Iterator, std::map< std::string, boost::any >(), ascii::space_type > object_rule;
	qi::rule< Iterator, std::pair< std::string, boost::any >(), ascii::space_type > pair_rule;
	qi::rule< Iterator, boost::any(), ascii::space_type > value_rule;
	qi::rule< Iterator, std::vector< boost::any >(), ascii::space_type > array_rule;
	qi::rule< Iterator, std::string(), ascii::space_type > string_rule;
};

}

using json::Value;

class Value::Private {
public:
	boost::any value;
};

Value Value::load( const std::string & in ) {
	Value tmp;
	Grammar< std::string::const_iterator > g;
	bool r = qi::phrase_parse( in.begin(), in.end(), g, ascii::space, tmp.p_->value );
	if( !r ) {
		assert( !"parse failed" );
	}
	return tmp;
}

Value::Value(): p_( new Private ) {
}

Value::Value( std::shared_ptr< Private > p ): p_( p ) {
}

bool Value::isObject() const {
	try {
		boost::any_cast< Object_ >( this->p_->value );
		return true;
	} catch( boost::bad_any_cast & e ) {
		return false;
	}
}

bool Value::isArray() const {
	try {
		boost::any_cast< Array_ >( this->p_->value );
		return true;
	} catch( boost::bad_any_cast & e ) {
		return false;
	}
}

bool Value::isNull() const {
	try {
		boost::any_cast< void * >( this->p_->value );
		return true;
	} catch( boost::bad_any_cast & e ) {
		return false;
	}
}

bool Value::isString() const {
	try {
		boost::any_cast< std::string >( this->p_->value );
		return true;
	} catch( boost::bad_any_cast & e ) {
		return false;
	}
}

std::vector< Value > Value::toArray() const {
	Array_ v;
	try {
		v = boost::any_cast< Array_ >( this->p_->value );
	} catch( boost::bad_any_cast & e ) {
		return std::vector< Value >();
	}
	std::vector< Value > tmp( v.size() );
	for( auto i = 0u; i < tmp.size(); ++i ) {
		tmp[i].p_->value = v[i];
	}
	return tmp;
}

std::map< std::string, Value > Value::toObject() const {
	Object_ o;
	try {
		o = boost::any_cast< Object_ >( this->p_->value );
	} catch( boost::bad_any_cast & e ) {
		return std::map< std::string, Value >();
	}
	std::map< std::string, Value > tmp;
	for( auto it = o.begin(); it != o.end(); ++it ) {
		auto p = std::make_shared< Private >();
		p->value = it->second;
		tmp.insert( std::make_pair( it->first, Value( p ) ) );
	}
	return tmp;
}

std::string Value::toString() const {
	std::string s;
	try {
		s = boost::any_cast< std::string >( this->p_->value );
	} catch( boost::bad_any_cast & e ) {
		return std::string();
	}
	return s;
}
