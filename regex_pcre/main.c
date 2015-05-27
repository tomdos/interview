#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include "main.h"
#include "re.h"


/*
 * Resize a storage for tokens' id and tokens' location. 
 */
void
token_storage_realloc(token_t *tokens, size_t size)
{
	tokens->storage = (uint32_t *) realloc(tokens->storage, (tokens->size + size) * sizeof(uint32_t));
	assert(tokens->storage);
	tokens->tcs = (token_cap_seq_t *) realloc(tokens->tcs, (tokens->size + size) * sizeof(token_cap_seq_t));
	assert(tokens->tcs);
	
	tokens->size += size;
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
input_pattern_parser_token(const char *start, char *rstring, size_t rstring_len, token_t *tokens)
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
	 * Store token identifier and its position
	 */
	if (tokens->len >= tokens->size)
		token_storage_realloc(tokens, TOKEN_RESIZE);
		
	tokens->storage[tokens->len] = id;
	tokens->tcs[tokens->len].start = start;
	tokens->tcs[tokens->len].len = p - start;
	tokens->len++;	
	
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
input_pattern_parser(const char *pattern, token_t *tokens)
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
			ret = input_pattern_parser_token(&pattern[i], regex_token, GENERAL_BUFSIZE, tokens);
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


//FIXME - long lines
char *
input_read()
{
	static char buf[GENERAL_BUFSIZE];
	size_t len;

	//FIXME - long line, input check
	fgets(buf, GENERAL_BUFSIZE, stdin);
	len = strlen(buf);
	//assert(buf[len-1] == '\n');
	buf[len-1]='\0'; // remove nl

	if (feof(stdin))
		return NULL;

	return buf;
}


void
debug_print(glb_t *glb)
{
	int i;
	re_posix_t *re_posix;
	token_t *tokens;
	char *input_line;
	char *input_pattern;
	
	re_posix = &glb->re_posix;
	tokens = &glb->tokens;
	input_line = glb->input_line;
	input_pattern = glb->input_pattern;
	
	printf("Input:   '%s'\nPattern: '%s'\nRegex:   '%s'\n", 
		input_line, glb->input_pattern, glb->regex);
	
	printf("Token captures:\n");
	for (i = 1; i < re_posix->pmatch_size && re_posix->pmatch[i].rm_so != -1; i++) {
		
		printf(" %3d: '%.*s' =~ '%.*s'\n", 
			tokens->storage[i-1],
			tokens->tcs[i-1].len,
			tokens->tcs[i-1].start,
			re_posix->pmatch[i].rm_eo - re_posix->pmatch[i].rm_so, 
			&input_line[re_posix->pmatch[i].rm_so]);	
	}
}


void
init(glb_t *glb)
{
	//alloc
	memset(glb, 0, sizeof(glb_t));
}


void
fini(glb_t *glb)
{
	
}


int
main(int argc, char *argv[])
{
	//pcre2_code *re;
	int ret;
	glb_t glb;
	
	init(&glb);
	
/* USE STDIO */
#if USE_INPUT_STDIO
	if (argc != 2) {
		fprintf(stderr, "Usage: regex <pattern>\n");
		exit(1);
	}

	input_pattern = argv[1];
	regex = input_pattern_parser(input_pattern);
	re_posix_comp(&glb.re_posix, regex);

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
	
	glb.input_pattern = argv[1];
	glb.input_line = argv[2];
	
	glb.regex = input_pattern_parser(glb.input_pattern, &glb.tokens);
	re_posix_init(&glb.re_posix, glb.tokens.len);
	
	ret = re_posix_comp(&glb.re_posix, glb.regex);
	ret = re_posix_exec(&glb.re_posix, glb.input_line);		
	
	#if USE_VERBOSE
		if (ret == 1) {
			printf("Matched: YES\n");
			debug_print(&glb);
		}
		else if (ret == 0) {
			printf("Matched: NO\n");
		}
		else {
			fprintf(stderr, "Matched: error\n");
		}
		
	#else
		if (ret == 1)
			printf("%s\n", glb.input_line);
	#endif
		
#endif	

	fini(&glb);
	
	return 0;
}
