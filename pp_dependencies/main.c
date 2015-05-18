#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <ctype.h>

#define BUFSIZE         3
#define ALPHABETSIZE    26
#define VECTORSIZE      (ALPHABETSIZE+1)    /* size of alphabet + trailing 0 */
#define TABLESIZE       10
#define ASCII_A         65

#define is_newline(buf) (buf[0] == '\n' || (buf[0] == '\r' && buf[1] == '\n'))

/*
 * Main structure which keeps all data.
 */
typedef struct {
  char **table;         /* Table contains dependencies, first element is 'id'. */
} dep_table_t;


/* Debug print */
void
dep_table_print(dep_table_t *dep_table)
{
  size_t i;

  for (i = 0; i < ALPHABETSIZE; i++)
    if (dep_table->table[i])
      printf("%s\n", dep_table->table[i]);
}


/* Main init of table structure */
dep_table_t *
dep_table_init()
{
  dep_table_t *dt;

  dt = (dep_table_t *) malloc(sizeof(dep_table_t));
  assert(dt);

  /* array */
  dt->table = (char **) calloc(1, ALPHABETSIZE * sizeof(char *));
  assert(dt->table);

  return dt;
}


/* Clean up - free table structure */
void
dep_table_fini(dep_table_t *dep_table)
{
  size_t i;

  for (i = 0; i < ALPHABETSIZE; i++)
    if (dep_table->table[i])
      free(dep_table->table[i]);

  free(dep_table->table);
  free(dep_table);
}


/*
 * Save vector.
 * It is assumed that slot is not occupied.
 */
void
dep_table_fill(dep_table_t *dep_table, char *vector)
{
  size_t idx;

  idx = (unsigned int) vector[0] - ASCII_A;

  assert(dep_table->table[idx] == NULL);
  dep_table->table[idx] = strdup(vector);
  assert(dep_table->table[idx]);
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

  v = 0;
  whitespace = 1;
  for (i = 0; line[i] != '\0'; i++) {
    if (i >= vector_size - 1) {
      fprintf(stderr, "Wrong format of line: %s", line);
      return 1;
    }

    if (line[i] == ' ' || line[i] == '\t' || line[i] == '\n') {
      whitespace = 1;
      continue;
    }

    if (!isalpha(line[i])) {
      fprintf(stderr, "Wrong format of line: %s", line);
      return 1;
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


/*
 * Read standard input.
 */
int
input_read(dep_table_t *dep_table)
{
  char buf[BUFSIZE];
  char *big_buf;
  char vector[VECTORSIZE];
  size_t big_buf_size;
  size_t len;

  big_buf = NULL;
  big_buf_size = input_realloc_buf(&big_buf, 0);

  while (1) {
    /*
     * Solution for a long long line.
     */
    memset(big_buf, 0, big_buf_size);
    while (1) {
      fgets(buf, BUFSIZE, stdin);
      strncat(big_buf, buf, big_buf_size);
      len = strlen(buf);
      if (buf[len-1] == '\n') {
        break;
      }

      big_buf_size = input_realloc_buf(&big_buf, big_buf_size);
    }

    if (feof(stdin) || is_newline(big_buf))
      break;

    if (input_parse(big_buf, vector, VECTORSIZE))
      return 1;

    dep_table_fill(dep_table, vector);
  }

  free(big_buf);

  return 0;
}


/*
 * Return table row (dependency) for input letter. Skip first letter.
 */
char *
dfs_get_row(dep_table_t *dep_table, char letter)
{
  char *new_vector;

  new_vector = dep_table->table[(unsigned int) letter - ASCII_A];
  if (new_vector)
    new_vector++;

  return new_vector;
}


/*
 * Mark single dependency (letter)
 */
void
dfs_mark(uint8_t *dep_vector, char letter)
{
  size_t idx;

  idx =  (unsigned int) letter - ASCII_A;
  dep_vector[idx] = 1;
}


/*
 * DFS - iterate through all letters and try to go deeper.
 */
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


/*
 * Print dependency
 */
void
dependency_print(dep_table_t *dep_table)
{
  size_t i, j;
  uint8_t dep_vector[ALPHABETSIZE];

  for (i = 0; i < ALPHABETSIZE; i++) {
    if (dep_table->table[i] == NULL)
      continue;

    memset(dep_vector, 0, sizeof(dep_vector));
    dfs(dep_table, dep_vector, &(dep_table->table[i][1]));

    /* Print dependency line */
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
