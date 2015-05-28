#ifndef __MAIN_H
#define __MAIN_H

#include <stdint.h>
#include "re.h"

#define		GENERAL_BUFSIZE		512   /* General buffer size */
#define		USE_INPUT_STDIO		1     /* Read input from stdio (or use args if 0) */
#define		USE_VERBOSE				0     /* Verbose (debug) output */


/* 
 * Escape sequeces for reqular text input. It may happened that input pattern 
 * contains real regual expression. That will lead to unexpected result. Therefore 
 * whole input pattern needs to be escaped.
 *
 * Example 
 * Pattern input: "(.*)%{1}$"
 * Parsed regex: '^\Q(.*)\E(.*?)\Q$\E$'
 * Will match: '(.*) A B C$'
 */
#define   PATTERN_ESCAPE_S      "\\Q"  
#define   PATTERN_ESCAPE_E      "\\E"

/* Begin anchor and end anchor */
#define   PATTERN_BEGIN         "^"PATTERN_ESCAPE_S
#define   PATTERN_BEGIN_LEN     3
#define   PATTERN_END           PATTERN_ESCAPE_E"$"
#define   PATTERN_END_LEN       3
/* */
#define		PATTERN_ESCAPE_TOKEN	"%%"
/* Lazy quantifier */
#define		PATTERN_WORD		      PATTERN_ESCAPE_E"(.*?)"PATTERN_ESCAPE_S 
/* more then one space */
#define		PATTERN_SPACE		      PATTERN_ESCAPE_E"((?:[^[:space:]]*\\s[^[:space:]]*){%u})"PATTERN_ESCAPE_S 
/* space modifier - no space */
#define		PATTERN_NOSPACE	      PATTERN_ESCAPE_E"([^[:space:]]*)"PATTERN_ESCAPE_S 
/* Greedy */
#define		PATTERN_GREEDY	      PATTERN_ESCAPE_E"(.*)"PATTERN_ESCAPE_S

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
  char *input_line;           /* Input text from stdio */
  char *input_pattern;        /* Input pattern */
  char *regex;                /* Valid regex generated from input_pattern */
  size_t input_line_size;     /* Buffer size */
  size_t regex_size;          /* Buffer size */
  token_t tokens;             /* Token structure - all info about input tokens */
  re_posix_t re_posix;        /* Structure used by PCRE wrappers */
} glb_t;


#endif