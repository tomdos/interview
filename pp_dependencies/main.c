#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define BUFSIZE         100
#define VECTORSIZE      27    /* size of alphabet + trailing 0 */
#define TABLESIZE       10

//FIXME - array x pointer ..
#define is_newline(buf) (buf[0] == '\n' || (buf[0] == '\r' && buf[1] == '\n'))

typedef struct {
  char **table;
  size_t table_idx;
  size_t table_size;
} dep_table_t;

dep_table_t *dep_table;


void
dep_table_print(dep_table_t *dep_table)
{
  size_t i;

  for (i = 0; i < dep_table->table_idx; i++)
    printf("%s\n", dep_table->table[i]);
}

dep_table_t *
dep_table_init()
{
  dep_table_t *dt;

  dt = (dep_table_t *) calloc(1, sizeof(dep_table_t));
  assert(dt);

  return dt;
}

void
dep_table_fini(dep_table_t **dep_table)
{

}

void
dep_table_resize(dep_table_t *dep_table)
{
  char **nt;
  size_t size;

  size = dep_table->table_size + TABLESIZE;
  nt = (char **) realloc(dep_table->table, size * sizeof(char *));
  assert(nt);
  dep_table->table = nt;
  dep_table->table_size = size;
}

int
dep_table_fill(dep_table_t *dep_table, char *vector)
{
  //size_t len;
  //char *table_vector;

  if (dep_table->table_idx == dep_table->table_size)
    dep_table_resize(dep_table);

  dep_table->table[dep_table->table_idx] = strdup(vector);
  assert(dep_table->table[dep_table->table_idx]);
  dep_table->table_idx++;

  //printf("%s\n", vector);

  return 0;
}

/*
 * Building wheel: Functions like strtok or strsep didn't work as I desired.
 */
int
input_parse(char *line, char *vector, size_t vector_size)
{
  //char vector[ALPHABETSIZE + 1];
  char *token;
  uint8_t whitespace;
  int i, v;

  //FIXME - better iteration - skip whitespace - be carefull about end
  //FIXME - check input - only letters (no num, no chars ...)
  v = 0;
  whitespace = 1;
  for (i = 0; line[i] != '\0'; i++) {
    if (i >= vector_size - 1) {
      fprintf(stderr, "Wrong format of line: %s", line);
      return 1;
    }

    if (line[i] == ' ' || line[i] == '\t' || line[i] == '\n') { //FIXME - use is_neline win x lin ...
      whitespace = 1;
      continue;
    }

    if (!whitespace) {
      fprintf(stderr, "Wrong format of line: %s", line);
      return 1;
    }

    whitespace = 0;

    vector[v] = line[i];
    v++;
  }

  vector[v] = '\0';

  return 0;
}

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

int
input_read(dep_table_t *dep_table)
{
  char *buf;
  char vector[VECTORSIZE];
  size_t buf_size;
  size_t ret;

  buf = NULL;
  buf_size = input_realloc_buf(&buf, 0);

  while (1) {
    // FIXME - realloc
    fgets(buf, buf_size, stdin);
    //printf("%s", buf);

    if (feof(stdin) || is_newline(buf))
      break;

    input_parse(buf, vector, VECTORSIZE);
    dep_table_fill(dep_table, vector);
    //printf("%s\n", vector);
  }

  return 0;
}




int
main(int argc, char *argv[])
{
  dep_table_t *dep_table;

  dep_table = dep_table_init();

  input_read(dep_table);
  dep_table_print(dep_table);


  return 0;
}
