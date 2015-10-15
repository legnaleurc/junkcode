#ifndef JSON_HPP
#define JSON_HPP

#include <vector>
#include <map>
#include <string>
#include <memory>

namespace json {

class Value {
public:
	static Value load( const std::string & in );

	Value();

	bool isArray() const;
	bool isObject() const;
	bool isNull() const;
	bool isString() const;

	std::vector< Value > toArray() const;
	std::map< std::string, Value > toObject() const;
	std::string toString() const;

private:
	class Private;

	explicit Value( std::shared_ptr< Private > );

	std::shared_ptr< Private > p_;
};

}

#endif
