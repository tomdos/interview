#define PCRE2_CODE_UNIT_WIDTH 8
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <pcre2.h>
#include "re.h"

#define		BUFSIZE						512
#define		USE_PCRE2POSIX		1
#define		USE_INPUT_STDIO		1
#define		USE_VERBOSE				0

//FIXME - * at least something (shoud be used + insted)
#if USE_PCRE2POSIX
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

#if 0
/* Alloc/realloc input buffer */
int
input_realloc_buf(char **buf, size_t size)
{
	size_t new_size;
	char *new_buf;

	assert(size >= 0);
	new_size = size + BUFSIZE;

	new_buf = realloc(*buf, new_size);
	if (new_buf == NULL) {
		if (size > 0)
			free(buf);

		perror("Input alloc failed.\n");
		exit(1);
	}

	*buf = new_buf;

	return new_size;
}
#endif


/*
 * Parse input token.
 * Use proper regex based on a token stored in start.
 * Return length of input token and proper regex stored in rstring 
 * or -1 if error occures.
 */
int 
input_pattern_parser_token(const char *start, char *rstring, size_t rstring_len)
{
	uint8_t greedy;
	uint8_t whitespace;
	unsigned id;
	unsigned whitespace_num;
	int ret;
	int len;
	const char *p;

	greedy = 0;
	whitespace = 0;
	p = start;
	if (*p++ != '%') return -1;
	if (*p++ != '{') return -1;
	
	/* Identifier */
	ret = sscanf(p, "%u%n", &id, &len); 
	if (ret != 1) return -1;
	p += len; // jump behind processed number
	
	/* Whitespace */
	if (*p == 'S') { 
		whitespace = 1;
		p++;
		ret = sscanf(p, "%u%n", &whitespace_num, &len); // identifier
		if (ret != 1) return -1;
		p += len;
	}
	
	/* Greedy */
	else if (*p == 'G') {
		p++;
		greedy = 1;
	}
	
	if (*p++ != '}') return -1;
	
	/* 
	 * Return proper format of regex in rstring. 
	 */
	if (greedy) {
		ret = snprintf(rstring, rstring_len, PATTERN_GREEDY);
		assert(ret > 0 && ret < rstring_len);
	}
	else if (whitespace) {
		if (whitespace_num == 0)
			ret = snprintf(rstring, rstring_len, PATTERN_NOSPACE);
		else
			ret = snprintf(rstring, rstring_len, PATTERN_SPACE, whitespace_num);
		assert(ret > 0 && ret < rstring_len);
	}
	else {
		ret = snprintf(rstring, rstring_len, PATTERN_WORD);
		assert(ret > 0 && ret < rstring_len);
	}
	
	return p - start; //return token len
}

/*
 * Parse input pattern (whole input pattern string) and return supported PCRE regex.
 *
 * Supported regex:
 *  (\S*) - word with no space
 *  ((\S*\s\S*){N} - exactly N spaces
 *  (.*) - greedy
 *
 */
char *
input_pattern_parser(const char *pattern)
{
	char *regex;
	char regex_token[BUFSIZE];
	size_t len;
	size_t i, j;
	int ret;
	
	//FIXME - realloc the buffer 
	regex = malloc(BUFSIZE);
	assert(regex);
	
	
	//FIXME - check buffer len
	i = 0;
	j = 0;
	regex[j++] = '^';
	while (pattern[i]) {
		//FIXME - escape
		if (pattern[i] == '%') {
			ret = input_pattern_parser_token(&pattern[i], regex_token, BUFSIZE);
			if (ret == -1)
				return NULL;
				
			i += ret;
			strcat(&regex[j], regex_token); //FIXME
			j += strlen(regex_token);
		}
		else {
			regex[j] = pattern[i];
			i++;
			j++;
		}
	}
	regex[j] = '$';
	
	return regex;
}

int
process_line(const char *line, const char *pattern)
{
	printf("%s - %s\n", line, pattern);
	return 0;
}

char *
input_read()
{
	static char buf[BUFSIZE];
	size_t len;

	//FIXME - long line, input check
	fgets(buf, BUFSIZE, stdin);
	len = strlen(buf);
	buf[len-1]='\0'; // remove nl


	if (feof(stdin))
		return NULL;

	return buf;
}



int
main(int argc, char *argv[])
{
	char *input_pattern;
	char *input_line;
	char *regex;
	pcre2_code *re;
	regex_t *preg;
	int ret;
	

#if USE_INPUT_STDIO
	if (argc != 2) {
		fprintf(stderr, "Usage: regex <pattern>\n");
		exit(1);
	}

	input_pattern = argv[1];
	regex = input_pattern_parser(input_pattern);
	preg = re_posix_comp(regex);

#if USE_VERBOSE
	printf("regex: '%s'\n", regex);
#endif
	
	while ((input_line = input_read())) {
		ret = re_posix_exec(preg, input_line);
 #if USE_VERBOSE
		if (ret == 1)
			printf("%s - YES\n", input_line);
		else if (ret == 0)
			printf("%s - NO\n", input_line);
 #else
		if (ret == 1)
			printf("%s\n", input_line);
 #endif
	}
	
#else
	if (argc != 3) {
		fprintf(stderr, "Usage: regex <pattern> <string>\n");
		exit(1);
	}
	
	input_pattern = argv[1];
	input_line = argv[2];
	regex = input_pattern_parser(input_pattern);
	printf("%s - %s\n", input_line, regex);
	
	//re = re_patter_init(regex);
	//re_pattern_match(re, input_line);	
	
	preg = re_posix_comp(regex);
	re_posix_exec(preg, input_line);		
#endif	
	
	return 0;
}
