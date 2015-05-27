#define PCRE2_CODE_UNIT_WIDTH 8
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
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


/* Alloc/realloc input buffer */
size_t
input_read_realloc(char **buf, size_t size)
{
	size_t new_size;
	char *new_buf;

	/* New buff alloc */
	if (size == 0)
		*buf = NULL;
		
	new_size = size + GENERAL_BUFSIZE;

	new_buf = realloc(*buf, new_size);
	assert(new_buf);
	*buf = new_buf;

	return new_size;
}



/*
 * Parse input token.
 * Use proper regex based on a token stored in start arg.
 * Return length of input token and proper regex stored in rstring 
 * or -1 if error occures.
 * 
 * Expected input:
 * %{N}
 * %{NG}
 * %{NSN}
 * %%
 * 
 */
int 
input_pattern_parser_token(char *start, char *rstring, size_t rstring_len)
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
	/* 
	 * Begin 
	 */
	if (*p++ != '%') return -1;
	/* 
	 * Escaping - if we escape % by %% we need to copy just single % into final regex
	 * and jump over %% in input string. Therefore 2 is returned although just signle
	 * character is in rstring buffer.
	 *
	 */
	if (*p == '%') {
		ret = snprintf(rstring, rstring_len, PATTERN_ESCAPE);
		assert(ret > 0 && ret < rstring_len);
		return 2;
	}
	if (*p++ != '{') return -1;
	
	/* 
	 * Token identifier 
	 */
	ret = sscanf(p, "%u%n", &id, &len); 
	if (ret != 1) return -1;
	p += len; // jump behind processed number
	
	/* 
	 * Whitespace 
	 */
	if (*p == 'S') { 
		whitespace = 1;
		p++;
		ret = sscanf(p, "%u%n", &whitespace_num, &len); // identifier
		if (ret != 1) return -1;
		p += len;
	}
	
	/* 
	 * Greedy 
	 */
	else if (*p == 'G') {
		p++;
		greedy = 1;
	}
	
	/* 
	 * End 
	 */
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
 * Add src to dest. If dest' size is not big enough then realloc dest.
 * Return dest or realloc'd dest with content of src.
 */
char *
memory_concat(char *dest, size_t dest_pos, size_t *dest_size, const char *src, size_t src_size)
{
	char *new_dest;
	size_t new_size;
	
	new_dest = dest;
	
	/* Resize input buffer is necessary. */
	if ((*dest_size - dest_pos) < src_size) {
		new_size = (GENERAL_BUFSIZE < src_size) ? GENERAL_BUFSIZE + src_size : 1;
		new_dest = (char *) realloc(dest, new_size);
		assert(new_dest);
		
		*dest_size = new_size;
	}
	
	/* Must always fit because of above test. */
	memcpy(&new_dest[dest_pos], src, src_size);
	
	return new_dest;
}


/*
 * Parse input pattern (whole input pattern string) and return supported PCRE regex.
 * Return alloc'd string or NULL in case of error.
 * Return value muset be freed!
 */
char *
input_pattern_parser(char *pattern)
{
	char *regex;
	size_t regex_size;
	char regex_token[GENERAL_BUFSIZE];
	size_t len;
	size_t i, j;
	int ret;
	
	//FIXME - realloc the buffer 
	regex_size = GENERAL_BUFSIZE;
	regex = (char *) malloc(sizeof(char) * regex_size);
	assert(regex);
	
	//FIXME - check buffer len
	i = 0;
	j = 0;
	//regex[j++] = '^';
	regex = memory_concat(regex, j, &regex_size, "^", 1); 
	j++;
	while (pattern[i]) {
		//FIXME - escape
		if (pattern[i] == '%') {
			ret = input_pattern_parser_token(&pattern[i], regex_token, GENERAL_BUFSIZE);
			if (ret == -1)
				return NULL;
				
			i += ret;
			len = strlen(regex_token);
			//strcat(&regex[j], regex_token); //FIXME
			regex = memory_concat(regex, j, &regex_size, regex_token, len);
			j += len;
		}
		else {
			//regex[j] = pattern[i];
			regex = memory_concat(regex, j, &regex_size, &pattern[i], 1);
			i++;
			j++;
		}
	}
	//regex[j] = '$';
	//FIXME null??
	
	regex = memory_concat(regex, j, &regex_size, "$", 1);
	j++;
	regex = memory_concat(regex, j, &regex_size, "\0", 1);
	
	return regex;
}


char *
input_read()
{
	static char buf[GENERAL_BUFSIZE];
	size_t len;
	//size_t idx;

	//FIXME - long line, input check
	fgets(buf, GENERAL_BUFSIZE, stdin);
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
	
/* USE STDIO */
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
	
/* USE ARGS */
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
		
#endif	
	
	return 0;
}
