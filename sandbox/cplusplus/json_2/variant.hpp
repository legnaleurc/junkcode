#ifndef VARIANT_HPP
#define VARIANT_HPP


#include <string>
#include <vector>
#include <unordered_map>
#include <memory>


class Variant;
typedef std::wstring String;
typedef std::vector<Variant> VariantArray;
typedef std::unordered_map<String, Variant> VariantObject;


class Variant {
public:
  Variant ();
  explicit Variant (bool);
  explicit Variant (int);
  explicit Variant (const String &);
  explicit Variant (const VariantArray &);
  explicit Variant (const VariantObject &);

//   Variant (const Variant &);
//   Variant & operator = (const Variant &);

  bool asBool() const;
  int asInteger () const;
  String asString () const;
  VariantArray asArray () const;
  VariantObject asObject () const;

private:
  class Private;
  friend class Private;

  std::shared_ptr<Private> _;
};


Variant parse_json (const String & json);


#endif
