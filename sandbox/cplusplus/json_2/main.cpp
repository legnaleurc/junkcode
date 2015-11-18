#include "variant.hpp"

#include <iostream>
#include <fstream>


String read_json ();


int main (int argc, char **argv) {
  String json = read_json();
  auto object = parse_json(json).asObject();
  for (const auto & p : object) {
    ;
  }
  return 0;
}


String read_json () {
//   String json = L"{\"number\": 42, \"boolean\": true, \"list\": [null, 42, false]}";
  std::wifstream fin("/tmp/events.json");
  fin.imbue(std::locale("en_US.UTF-8"));
  String json((std::istreambuf_iterator<wchar_t>(fin)), std::istreambuf_iterator<wchar_t>());
  std::wclog << json << std::endl;
  return json;
}

