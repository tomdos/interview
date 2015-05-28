#ifndef _RE_H
#define _RE_H

#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>
#include <pcre2posix.h>


typedef struct 
{
  regex_t *preg;        /* Compiled regex */
  regmatch_t *pmatch;   /* Storage for results */
  size_t pmatch_size;   /* Size of above storage */
} re_posix_t;


void re_posix_init(re_posix_t *re_posix, size_t pmatch_size);
void re_posix_fini(re_posix_t *re_posix);
int re_posix_comp(re_posix_t *re_posix, const char *regex);
int re_posix_exec(re_posix_t *re_posix, const char *subject);

#endif