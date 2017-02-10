#include "c.h"

#include <regex>


int c_regex_match (const char * utf8_pattern, const char * utf8_cell) {
    std::regex pattern(utf8_pattern);
    bool rv = std::regex_match(utf8_cell, pattern);

    return rv ? 1 : 0;
}
