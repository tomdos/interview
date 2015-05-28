#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "re.h"
#include "main.h"

/*
 * Initialize structures
 */
void
re_posix_init(re_posix_t *re_posix, size_t pmatch_size)
{
  re_posix->preg = (regex_t *) malloc(sizeof(regex_t));
  assert(re_posix->preg);

  re_posix->pmatch_size = pmatch_size + 1; /* first match is regex itself */
  if (pmatch_size > 0) {
    re_posix->pmatch = (regmatch_t *) malloc(sizeof(regmatch_t) * re_posix->pmatch_size);
    assert(re_posix->pmatch);
  }
  else {
    re_posix->pmatch = NULL;
  }
}


/*
 * Clean up
 */
void
re_posix_fini(re_posix_t *re_posix)
{
  regfree(re_posix->preg);
  free(re_posix->preg);
  free(re_posix->pmatch);
}


/*
 * Compile valid regex to PCRE format.
 */
int
re_posix_comp(re_posix_t *re_posix, const char *regex)
{
  int ret;
  char errbuf[GENERAL_BUFSIZE];
  
  ret = regcomp(re_posix->preg, regex, 0);
  if (ret) {
    assert(
      regerror(ret, re_posix->preg, errbuf, GENERAL_BUFSIZE) < GENERAL_BUFSIZE
    );
    fprintf(stderr, "posix comp error: %s\n", errbuf);
  }
  
  return ret;
}


/*
 * Execute PCRE 
 */
int
re_posix_exec(re_posix_t *re_posix, const char *subject)
{
  char errbuf[GENERAL_BUFSIZE];
  int ret;
  int i;
  
  ret = regexec(re_posix->preg, subject, re_posix->pmatch_size, re_posix->pmatch, 0);
  if (ret != 0 && ret != REG_NOMATCH) {
    assert(
      regerror(ret, re_posix->preg, errbuf, GENERAL_BUFSIZE) < GENERAL_BUFSIZE
    );
    fprintf(stderr, "%s\n", errbuf);
    return -1;
  }
  
  return (REG_NOMATCH == ret) ? 0 : 1;
}

