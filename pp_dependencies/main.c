#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <ctype.h>

#define BUFSIZE         100
#define ALPHABETSIZE    26
#define VECTORSIZE      (ALPHABETSIZE+1)    /* size of alphabet + trailing 0 */
#define TABLESIZE       10
#define ASCII_A         65

//FIXME array pointer
#define is_newline(buf) (buf[0] == '\n' || (buf[0] == '\r' && buf[1] == '\n'))


typedef struct {
  char **table;         /* Table contains dependencies, first element is 'id'. */
  size_t table_idx;     /* Next index (used size)*/
  size_t table_size;    /* Total alloc'd size */
} dep_table_t;


/* Debug print */
void
dep_table_print(dep_table_t *dep_table)
{
  size_t i;

  for (i = 0; i < dep_table->table_idx; i++)
    printf("%s\n", dep_table->table[i]);
}


/* Main init of table structure */
dep_table_t *
dep_table_init()
{
  dep_table_t *dt;

  dt = (dep_table_t *) calloc(1, sizeof(dep_table_t));
  assert(dt);

  return dt;
}


/* Clean up - free table structure */
void
dep_table_fini(dep_table_t *dep_table)
{
  size_t i;

  for (i = 0; i < dep_table->table_idx; i++)
    free(dep_table->table[i]);

  free(dep_table->table);
  free(dep_table);
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

void
dep_table_fill(dep_table_t *dep_table, char *vector)
{
  if (dep_table->table_idx == dep_table->table_size)
    dep_table_resize(dep_table);

  dep_table->table[dep_table->table_idx] = strdup(vector);
  assert(dep_table->table[dep_table->table_idx]);
  dep_table->table_idx++;
}

/*
 * Parse single line.
 * Functions like strtok or strsep didn't work as I want.
 */
int
input_parse(char *line, char *vector, size_t vector_size)
{
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

    vector[v] = toupper(line[i]);
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

    if (feof(stdin) || is_newline(buf))
      break;

    if (input_parse(buf, vector, VECTORSIZE))
      return 1;

    dep_table_fill(dep_table, vector);
  }

  return 0;
}

char *
dfs_get_row(dep_table_t *dep_table, char letter)
{
  size_t i;
  char *new_vector;

  new_vector = NULL;
  for (i = 0; i < dep_table->table_idx; i++) {
    if (dep_table->table[i][0] == letter) {
      new_vector = dep_table->table[i];
      new_vector++;
      break;
    }
  }

  return new_vector;
}

void
dfs_mark(uint8_t *dep_vector, char letter)
{
  size_t idx;

  idx =  (unsigned int) letter - ASCII_A;
  dep_vector[idx] = 1;
}

void
dfs(dep_table_t *dep_table, uint8_t *dep_vector, char *vector)
{
  char *new_vector;
  char letter;

  while (*vector != '\0') {
    letter = *vector;
    new_vector = dfs_get_row(dep_table, letter);
    if (new_vector)
      dfs(dep_table, dep_vector, new_vector);
    dfs_mark(dep_vector, letter);
    vector++;
  }
}

void
dependency_print(dep_table_t *dep_table)
{
  //FIXME - cycle
  size_t i, j;
  uint8_t dep_vector[ALPHABETSIZE];

  for (i = 0; i < dep_table->table_idx; i++) {
    memset(dep_vector, 0, sizeof(dep_vector));

    dfs(dep_table, dep_vector, &(dep_table->table[i][1]));

    printf("%c  ", dep_table->table[i][0]);

    for (j = 0; j < ALPHABETSIZE; j++)
      if (dep_vector[j])
        printf("%c ", (uint8_t) j + ASCII_A);

    printf("\n");
  }
}


int
main(int argc, char *argv[])
{
  dep_table_t *dep_table;

  dep_table = dep_table_init();

  if (!input_read(dep_table)) {
    //dep_table_print(dep_table);
    dependency_print(dep_table);
  }

  dep_table_fini(dep_table);

  return 0;
}
