#ifndef __MAIN_H
#define __MAIN_H

#include <stdint.h>
#include <pcre2.h>
#include "re.h"


#define		GENERAL_BUFSIZE		512
#define		USE_PCRE2POSIX		1
#define		USE_INPUT_STDIO		0
#define		USE_VERBOSE				1

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


#define			TOKEN_RESIZE    10

typedef struct {
  char *start;
  size_t len;
} token_cap_seq_t;

typedef struct
{
  uint32_t *storage;
  token_cap_seq_t *tcs;
  size_t len;
  size_t size;
} token_t;


typedef struct
{
  char *input_line;
  char *input_pattern;
  char *regex;
  size_t input_line_size;
  size_t regex_size;
  //uint8_t token_id_storage[TOKEN_STORAGE_SIZE];
  //uint8_t token_id_storage_len;
  token_t tokens;
  re_posix_t re_posix;
} glb_t;


#endif