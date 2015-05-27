#ifndef _RE_H
#define _RE_H

#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>
#include <pcre2posix.h>

typedef struct 
{
  regex_t *preg;        /* compiled regex */
  regmatch_t *pmatch;   /* storage for results */
  size_t pmatch_size;   /* size of above storage */
} re_posix_t;


pcre2_code * re_patter_init(char *regex);
int re_pattern_match(pcre2_code *re, char *input);

int re_posix_init(re_posix_t *re_posix, size_t pmatch_size);
void re_posix_fini(re_posix_t *re_posix);
int re_posix_comp(re_posix_t *re_posix, const char *regex);
int re_posix_exec(re_posix_t *re_posix, const char *subject);

#endif