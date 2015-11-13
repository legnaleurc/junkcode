#ifndef VARIANT_HPP
#define VARIANT_HPP


#include <string>
#include <vector>
#include <unordered_map>


class Variant;
typedef std::wstring String;
typedef std::vector<Variant> VariantList;
typedef std::unordered_map<String, Variant> VariantMap;


class Variant {
public:
  operator bool () const;
  operator int () const;
  operator String () const;
  operator VariantList () const;
  operator VariantMap () const;
};


Variant parse_json (const String & json);


#endif
