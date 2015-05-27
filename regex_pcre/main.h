#ifndef __MAIN_H
#define __MAIN_H

#include <stdint.h>
#include "re.h"

#define		GENERAL_BUFSIZE		512   /* General buffer size */
#define		USE_PCRE2POSIX		1     /* Use pcre2posix - posix PCRE wrapper */
#define		USE_INPUT_STDIO		1     /* Read input from stdio (or use args if 0) */
#define		USE_VERBOSE				1     /* Verbose (debug) output */

//FIXME - * at least something (shoud be used + insted)
#if USE_PCRE2POSIX
 #define		PATTERN_ESCAPE	"%%"
 #define		PATTERN_WORD		"(.*)"
//"([^[:space:]]*)"
 /* more then one space */
 #define		PATTERN_SPACE		"(([^[:space:]]*\\s[^[:space:]]*){%u})" 
 /* space modifier - no space */
 #define		PATTERN_NOSPACE	"([^[:space:]]*)" 
//"((\\s|[^[:space:]]+\\s|\\s[^[:space:]]+){%u})"
//"(([^[:space:]]*\\s){%u})" //working
//"(([^\\s]+[:space:][^[:space:]]*){%u})"
 #define		PATTERN_GREEDY	"(.*)"

#else
 #define		PATTERN_WORD		"(\\S*)"
 #define		PATTERN_SPACE		"((\\S*\\s\\S*){%u})"
 #define		PATTERN_GREEDY	"(.*)"
#endif

/* Default realloc size for token storage and tcs. */
#define			TOKEN_RESIZE    10


/* Token reference - keeps a pointer to token in input pattern and its length */
typedef struct {
  const char *start;
  size_t len;
} token_cap_seq_t;


/* It keeps all tokens id and references. */
typedef struct
{
  uint32_t *storage;      /* Storage for token id */
  token_cap_seq_t *tcs;   /* All references */
  size_t len;             /* Current usage */
  size_t size;            /* Size of buffers */
} token_t;


/* Main structure */
typedef struct
{
  const char *input_line;     /* Input text from stdio */
  const char *input_pattern;  /* Input pattern */
  char *regex;                /* Valid regex generated from input_pattern */
  size_t input_line_size;     /* Buffer size */
  size_t regex_size;          /* Buffer size */
  token_t tokens;             /* Token structure - all info about input tokens */
  re_posix_t re_posix;        /* Structure used by PCRE wrappers */
} glb_t;


#endif