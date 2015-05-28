#ifndef __MAIN_H
#define __MAIN_H

#include <stdint.h>
#include "re.h"

#define		GENERAL_BUFSIZE		512   /* General buffer size */
#define		USE_INPUT_STDIO		0     /* Read input from stdio (or use args if 0) */
#define		USE_VERBOSE				1     /* Verbose (debug) output */


#define		PATTERN_ESCAPE	"%%"
/* Lazy quantifier */
#define		PATTERN_WORD		"(.*?)" 
/* more then one space */
#define		PATTERN_SPACE		"(([^[:space:]]*\\s[^[:space:]]*){%u})" 
/* space modifier - no space */
#define		PATTERN_NOSPACE	"([^[:space:]]*)" 
/* Greedy */
#define		PATTERN_GREEDY	"(.*)"

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