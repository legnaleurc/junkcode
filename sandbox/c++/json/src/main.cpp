#include "json.hpp"

#include <vector>
#include <map>
#include <iostream>

int main() {
	const std::string source = "[ null ]";
	json::Value v = json::Value::load( source );
	return 0;
}
