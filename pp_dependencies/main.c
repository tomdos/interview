#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define BUFSIZE         100
#define ALPHABETSIZE    26

//FIXME - array x pointer ..
#define is_newline(buf) (buf[0] == '\n' || (buf[0] == '\r' && buf[1] == '\n'))


int
input_parse(char *line)
{
  char a,b,c;
  char vector[ALPHABERSIZE];
  char *token;
  char *input;
  int i, v;

  //sscanf(line, "%c %c %c", &a, &b, &c);
  //printf("%c-%c-%c\n", a,b,c);
  //while ((token = strsep(&input, " \t")) != NULL)


  //FIXME - better iteration - skip whitespace - be carefull about end
  //FIXME - check input - only letters (no num, no chars ...)
  v = 0;
  for (i = 0; line[i] != '\0'; i++) {
    if (line[i] == ' ' || line[i] == '\t' || line[i] == '\n') //FIXME - use is_neline win x lin ...
      continue;

    vector[v] = line[i];
    v++;
  }

  for (i = 0; i < v; i++)
    printf("%c ", vector[i]);
  printf("\n");

  return 0;
}

int
input_alloc(char **buf, size_t size)
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

int
input_read()
{
  char *buf;
  size_t buf_size;
  size_t ret;

  buf_size = input_alloc(&buf, 0);

  while (1) {
    // FIXME - realloc
    fgets(buf, buf_size, stdin);
    //printf("%s", buf);

    if (feof(stdin) || is_newline(buf))
      break;

    input_parse(buf);
  }


  return 0;
}



int
main(int argc, char *argv[])
{
  input_read();
  return 0;
}
