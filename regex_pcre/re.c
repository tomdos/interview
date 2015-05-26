#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "re.h"

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





regex_t *
re_posix_comp(const char *regex)
{
  regex_t *preg;
  int ret;
  
  preg = (regex_t *) malloc(sizeof(regex_t));
  assert(preg);
  
  ret = regcomp(preg, regex, 0);
  if (ret) {
    char errbuf[256];
    regerror(ret, preg, errbuf, 256);    
    fprintf(stderr, "posix comp error: %s\n", errbuf);
    exit(1);
  }
  
  return preg;
}


int
re_posix_exec(regex_t *preg, const char *subject)
{
  int ret;
  size_t nmatch;
  regmatch_t pmatch[3];
  int i;
  
  memset(pmatch, 0, sizeof(pmatch));
  
  ret = regexec(preg, subject, 3, pmatch, 0);
  if (ret != 0 && ret != REG_NOMATCH) {
    char errbuf[256]; //FIXME
    regerror(ret, preg, errbuf, 256);
    fprintf(stderr, "%s\n", errbuf);
    return -1;
  }
  
  return (REG_NOMATCH == ret) ? 0 : 1;
  
  printf("Match:\n");
  i=1;
  while (i < 3 && pmatch[i].rm_so != -1) {
    printf("\t'%.*s'\n", pmatch[i].rm_eo-pmatch[i].rm_so, &subject[pmatch[i].rm_so]);
    i++;
  }
  
}
