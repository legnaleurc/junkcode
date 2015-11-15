#include "variant.hpp"

#include <functional>
#include <stack>
#include <sstream>
#include <iostream>
#include <cassert>


Variant::operator VariantMap () const {
  return VariantMap();
}


#define PF(fn) std::bind(&Parser::fn, this, std::placeholders::_1)


class Parser {
public:
  typedef std::function<void (wchar_t)> ParserFunction;

  std::wistringstream sin;
  ParserFunction parse;
  std::wostringstream buffer;
  std::stack<ParserFunction> state_stack;

  Parser (const String & json): sin(json), parse(PF(parseValue)), buffer() {
    wchar_t c = L'\0';
    while (this->sin >> c) {
      this->parse(c);
    }
  }

  void popState () {
    this->parse = this->state_stack.top();
    this->state_stack.pop();
  }

  void pushState (ParserFunction fn) {
    this->state_stack.push(this->parse);
    this->parse = fn;
  }

  void parseValue (wchar_t c) {
    switch (c) {
      case L' ':
      case L'\t':
      case L'\r':
      case L'\n':
        break;
      case L'{':
        std::clog << "start object" << std::endl;
        this->pushState(PF(parseObject));
        break;
      case L'[':
        std::clog << "start array" << std::endl;
        this->pushState(PF(parseArray));
        break;
      case L'"':
        std::clog << "start string" << std::endl;
        this->pushState(PF(parseString));
        break;
      case L'0':
      case L'1':
      case L'2':
      case L'3':
      case L'4':
      case L'5':
      case L'6':
      case L'7':
      case L'8':
      case L'9':
        std::clog << "start number" << std::endl;
        this->sin.putback(c);
        this->pushState(PF(parseNumber));
        break;
      case L't':
        std::clog << "start true" << std::endl;
        this->sin.putback(c);
        this->pushState(PF(parseTrue));
        break;
      case L'f':
        std::clog << "start false" << std::endl;
        this->sin.putback(c);
        this->pushState(PF(parseFalse));
        break;
      case L'n':
        std::clog << "start null" << std::endl;
        this->sin.putback(c);
        this->pushState(PF(parseNull));
        break;
      case L',':
        std::clog << "value end" << std::endl;
        this->sin.putback(c);
        this->popState();
        break;
      default:
        assert(!__PRETTY_FUNCTION__);
        break;
    }
  }

  void parseObject (wchar_t c) {
    switch (c) {
      case L' ':
      case L'\t':
      case L'\r':
      case L'\n':
        break;
      case L'}':
        std::clog << "end object" << std::endl;
        this->popState();
        break;
      case L'"':
        std::clog << "start object key" << std::endl;
        this->pushState(PF(parseString));
        break;
      case L':':
        std::clog << "start object value" << std::endl;
        this->pushState(PF(parseValue));
        break;
      case L',':
        std::clog << "next object pair" << std::endl;
        break;
      default:
        assert(!__PRETTY_FUNCTION__);
        break;
    }
  }

  void parseArray (wchar_t c) {
    switch (c) {
      case L' ':
      case L'\t':
      case L'\r':
      case L'\n':
        break;
      case L']':
        std::clog << "end array" << std::endl;
        this->popState();
        break;
      case L',':
        std::clog << "next entry" << std::endl;
        break;
      default:
        std::clog << "start entry" << std::endl;
        this->sin.putback(c);
        this->pushState(PF(parseValue));
        break;
    }
  }

  void parseString (wchar_t c) {
    switch (c) {
      case L'"':
        std::clog << "end string" << std::endl;
        this->popState();
        break;
      case L'\\':
        std::clog << "start string escape" << std::endl;
        this->pushState(PF(parseStringEscape));
        break;
      default:
        std::wclog << c << std::endl;
        break;
    }
  }

  void parseStringEscape (wchar_t c) {
    switch (c) {
      case L'"':
        std::clog << "end string escape" << std::endl;
        this->popState();
        break;
      default:
        assert(!__PRETTY_FUNCTION__);
        break;
    }
  }

  void parseNumber (wchar_t c) {
    switch (c) {
      case L'0':
      case L'1':
      case L'2':
      case L'3':
      case L'4':
      case L'5':
      case L'6':
      case L'7':
      case L'8':
      case L'9':
        std::wclog << c << std::endl;
        break;
      case L' ':
      case L'\t':
      case L'\r':
      case L'\n':
        this->popState();
        break;
      case L',':
      case L'}':
      case L']':
        this->sin.putback(c);
        this->popState();
        break;
      default:
        assert(!__PRETTY_FUNCTION__);
        break;
    }
  }

  void parseTrue (wchar_t c) {
    if (c != L't') {
      assert(!"parseTrue");
    }
    this->sin >> c;
    if (c != L'r') {
      assert(!"parseTrue");
    }
    this->sin >> c;
    if (c != L'u') {
      assert(!"parseTrue");
    }
    this->sin >> c;
    if (c != L'e') {
      assert(!"parseTrue");
    }
    this->sin >> c;
    switch (c) {
      case L',':
      case L']':
      case L'}':
        std::clog << "end true" << std::endl;
        this->sin.putback(c);
        this->popState();
        break;
      default:
        assert(!"parseTrue");
        break;
    }
  }

  void parseFalse (wchar_t c) {
    if (c != L'f') {
      assert(!"parseFalse");
    }
    this->sin >> c;
    if (c != L'a') {
      assert(!"parseFalse");
    }
    this->sin >> c;
    if (c != L'l') {
      assert(!"parseFalse");
    }
    this->sin >> c;
    if (c != L's') {
      assert(!"parseFalse");
    }
    this->sin >> c;
    if (c != L'e') {
      assert(!"parseFalse");
    }
    this->sin >> c;
    switch (c) {
      case L',':
      case L']':
      case L'}':
        std::clog << "end false" << std::endl;
        this->sin.putback(c);
        this->popState();
        break;
      default:
        assert(!"parseFalse");
        break;
    }
  }

  void parseNull (wchar_t c) {
    if (c != L'n') {
      assert(!"parseNull");
    }
    this->sin >> c;
    if (c != L'u') {
      assert(!"parseNull");
    }
    this->sin >> c;
    if (c != L'l') {
      assert(!"parseNull");
    }
    this->sin >> c;
    if (c != L'l') {
      assert(!"parseNull");
    }
    this->sin >> c;
    switch (c) {
      case L',':
      case L']':
      case L'}':
        std::clog << "end null" << std::endl;
        this->sin.putback(c);
        this->popState();
        break;
      default:
        assert(!"parseNull");
        break;
    }
  }

};


Variant parse_json (const String & json) {
  Parser p(json);
  return Variant();
}
