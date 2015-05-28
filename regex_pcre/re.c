#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "re.h"
#include "main.h"

#define   GENERAL_BUFFER_SIZE   512

pcre2_code *
re_patter_init(char *regex)
{
  pcre2_code *re;
  int errornumber;
  int ret;
  PCRE2_SIZE erroroffset;
  PCRE2_UCHAR buffer[GENERAL_BUFFER_SIZE];
  PCRE2_SPTR pattern;
  
  pattern = (PCRE2_SPTR) regex;
  
  
  re = pcre2_compile(
    regex,
    PCRE2_ZERO_TERMINATED,
    0,
    &errornumber,
    &erroroffset,
    NULL);
    
  if (re == NULL) {
    assert( 
      pcre2_get_error_message(errornumber, buffer, sizeof(buffer)) > 0
    );
    printf("PCRE2 compilation failed at offset %d: %s\n", (int)erroroffset, buffer);
  }
  
  return re;
}


void
re_pattern_fini()
{
  
}

int
re_pattern_match(pcre2_code *re, char *input)
{
  PCRE2_SPTR subject;  
  size_t subject_length;
  pcre2_match_data *match_data;
  int rc;
  PCRE2_SIZE *ovector;


  match_data = pcre2_match_data_create_from_pattern(re, NULL);

  subject = (PCRE2_SPTR) input;
  subject_length = strlen(input);
  
  rc = pcre2_match(
    re,                   /* the compiled pattern */
    subject,              /* the subject string */
    subject_length,       /* the length of the subject */
    0,                    /* start at offset 0 in the subject */
    0,                    /* default options */
    match_data,           /* block for storing the result */
    NULL);                /* use default match context */
  
  /* Matching failed: handle error cases */
  
  if (rc < 0)
    {
    switch(rc)
      {
      case PCRE2_ERROR_NOMATCH: printf("No match\n"); break;
      /*
      Handle other special cases if you like
      */
      default: printf("Matching error %d\n", rc); break;
      }
    pcre2_match_data_free(match_data);   /* Release memory used for the match */
    pcre2_code_free(re);                 /* data and the compiled pattern. */
    return 1;
    }
  
  /* Match succeded. Get a pointer to the output vector, where string offsets are
  stored. */
  
  ovector = pcre2_get_ovector_pointer(match_data);
  printf("\nMatch succeeded at offset %d\n", (int)ovector[0]);
  
  
  /*************************************************************************
  * We have found the first match within the subject string. If the output *
  * vector wasn't big enough, say so. Then output any substrings that were *
  * captured.                                                              *
  *************************************************************************/
  
  /* The output vector wasn't big enough. This should not happen, because we used
  pcre2_match_data_create_from_pattern() above. */
  
  if (rc == 0)
    printf("ovector was not big enough for all the captured substrings\n");
  
  /* Show substrings stored in the output vector by number. Obviously, in a real
  application you might want to do things other than print them. */
  
  int i;
  for (i = 0; i < rc; i++)
    {
    PCRE2_SPTR substring_start = subject + ovector[2*i];
    size_t substring_length = ovector[2*i+1] - ovector[2*i];
    printf("%2d: '%.*s'\n", i, (int)substring_length, (char *)substring_start);
    }
    
  return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////

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
  char errbuf[GENERAL_BUFFER_SIZE];
  
  ret = regcomp(re_posix->preg, regex, 0);
  if (ret) {
    assert(
      regerror(ret, re_posix->preg, errbuf, GENERAL_BUFFER_SIZE) < GENERAL_BUFFER_SIZE
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
  char errbuf[GENERAL_BUFFER_SIZE];
  int ret;
  int i;
  
  ret = regexec(re_posix->preg, subject, re_posix->pmatch_size, re_posix->pmatch, 0);
  if (ret != 0 && ret != REG_NOMATCH) {
    assert(
      regerror(ret, re_posix->preg, errbuf, GENERAL_BUFFER_SIZE) < GENERAL_BUFFER_SIZE
    );
    fprintf(stderr, "%s\n", errbuf);
    return -1;
  }
  
  return (REG_NOMATCH == ret) ? 0 : 1;
}

