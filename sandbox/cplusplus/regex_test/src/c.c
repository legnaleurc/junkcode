#include "c.h"

#include <regex.h>

#include <stddef.h>


int c_regex_match(const char * utf8_pattern, const char * utf8_cell) {
    int rv = 0;
    regex_t pattern;

    rv = regcomp(&pattern, utf8_pattern, REG_NOSUB);
    if (rv != 0) {
        rv = 0;
        goto end;
    }

    rv = regexec(&pattern, utf8_cell, 0, NULL, 0);
    if (rv != 0) {
        rv = 0;
        goto free_regex;
    }

    rv = 1;
free_regex:
    regfree(&pattern);
end:
    return rv;
}
