#ifndef _RE_H
#define _RE_H

#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>
#include <pcre2posix.h>


pcre2_code * re_patter_init(char *regex);
int re_pattern_match(pcre2_code *re, char *input);

regex_t * re_posix_comp(const char *regex);
int re_posix_exec(regex_t *preg, const char *subject);

#endif