#include "variant.hpp"

#include <iostream>

int main(int argc, char **argv) {
  String json = L"{\"number\": 42, \"boolean\": true, \"list\": [null, 42, false]}";
  VariantMap object = parse_json(json);
  for (auto p : object) {
    ;
  }
  return 0;
}
