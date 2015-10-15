#include "json.hpp"

#include <iostream>

#include <cassert>

int main() {
	const std::string source = "[ null ]";
	json::Value v = json::Value::load( source );
	assert( v.isArray() || !"type error" );
	auto a = v.toArray();
	for( auto it = a.begin(); it != a.end(); ++it ) {
		assert( it->isNull() || "type error" );
	}
	return 0;
}
