#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE 50
#define MAX_WORDS 100

void error_fatal(const char *message);

/*********************************************************/
/* global variables                                      */
/*********************************************************/

int puzzle_size;
char puzzle[MAX_SIZE][MAX_SIZE];
char rows[MAX_SIZE][2 + MAX_SIZE];
char cols[MAX_SIZE][2 + MAX_SIZE];
char diag_up[2 * MAX_SIZE - 1][2 + MAX_SIZE];
char diag_down[2 * MAX_SIZE - 1][2 + MAX_SIZE];

int num_words;
char words[MAX_WORDS][1 + MAX_SIZE];

char matches[MAX_SIZE][MAX_SIZE];

/*********************************************************/
/* procedures for reading/parsing the file               */
/*********************************************************/

void read_puzzle(FILE *f) {
  /*
  Reads the contents of f into the puzzle[][] array. Also sets puzzle_size
  to the dimension of the N x N grid.
  */
  char c;
  int i;
  int j;
  
  /* read the first row */
  i = 0;
  j = 0;

  while (((c = fgetc(f)) != EOF) && (c != '\n'))
    if (c != ' ') {
      printf("%c ", c);
      puzzle[0][j++] = c;
      if (j > MAX_SIZE)
        error_fatal("Malformed puzzle - too many columns");
    }
  printf("\n");
  
  rows[0][1 + j] = '\0';

  /* puzzle size is the number of elements in the first row */
  puzzle_size = j;
  //printf("puzzle_size %d\n", puzzle_size);

  /* read the remaining N - 1 rows */
  i = 1;
  j = 0;

  while ((i < puzzle_size) && ((c = fgetc(f)) != EOF)) {
    if (c == '\n') {
      printf("\n");
      if (j != puzzle_size)
        error_fatal("Malformed puzzle - row too short");
      if (++i > MAX_SIZE)
        error_fatal("Malformed puzzle - too many rows");
      j = 0;
    }
    else if (c != ' ') {
      //printf("%d %d %c\n", i, j, c);
      printf("%c ", c);
      puzzle[i][j++] = c;
      if (j > puzzle_size + 1)
        error_fatal("Malformed puzzle - too many columns");
    }
  }
 
  if ((c != '\n') || (i != puzzle_size))
    error_fatal("Malformed puzzle - not enough rows");
}

void read_words(FILE *f) {
  /*
  Reads the contents of f into the words variable. Also sets num_words
  */
  char c;
  int i = 0;
  int j = 0;

  while ((c = fgetc(f)) != EOF) {
    if (c == '\n') {
      words[i++][j] = '\0';
      puts(words[i - 1]);
      if (i >= MAX_WORDS)
        error_fatal("Too many words");
      j = 0;
    }
    else {
      words[i][j++] = c;
      if (j > puzzle_size)
        error_fatal("Word too long");
    }
  }

  /* record number of words */
  num_words = i;
  //printf("num_words %d\n", num_words);
}

/*********************************************************/
/* functions for searching for words                     */
/*********************************************************/

void build_structures() {
  /*
  Builds the rows, cols, diag_up, and diag_down structures based on the
  values of puzzle[][] and puzzle_size.
  */
  int i;
  int j;

  /* rows and columns */
  for (i = 0; i < puzzle_size; i++) {
    for (j = 0; j < puzzle_size; j++) {
      //printf("%d %d %c\n", i, j, puzzle[i][j]);
      rows[i][1 + j] = puzzle[i][j];
      cols[i][1 + j] = puzzle[j][i];
    }
    //puts(1 + rows[i]);
    //puts(1 + cols[i]);
  }

  /* diagonals */
  for (i = 0; i < puzzle_size; i++) {
    for (j = 0; j <= i; j++) {
      diag_up[i][1 + j] = puzzle[i - j][j];
      diag_down[i][1 + j] = puzzle[puzzle_size - i + j - 1][j];
    }
    //puts(1 + diag_up[i]);
    //puts(1 + diag_down[i]);
  }
  for (i = 1 + puzzle_size; i < 2 * puzzle_size; i++) {
    for (j = 0; j < 2 * puzzle_size - i; j++) {
      diag_up[i][1 + j] = puzzle[puzzle_size - j - 1][i - puzzle_size + j];
      diag_down[i][1 + j] = puzzle[j][i - puzzle_size + j];
    }
    //puts(1 + diag_up[i]);
    //puts(1 + diag_down[i]);
  }
}

void search_words() {
  /*
  Searches for words. There are eight strings ways a word could be in the 
  puzzle:
    horizontal from left/right
    vertical from top/bottom
    diagonal from top-left/bottom-left
  */
  int i;
  int j;
  int k;
  int match;

  for (i = 0; i < num_words; i++) {
    for (j = 0; j < puzzle_size; j++) {
      /* horizontal from left? */
      if ((match = contains(rows[j], words[i])) != -1) {
        printf("%s in row %d\n", words[i], j);
        for (k = 0; words[i][k]; k++)
          matches[j][match + k] = words[i][k];
      }

      /* horizontal from right? */

      /* vertical from top? */
      if ((match = contains(cols[j], words[i])) != -1) {
        printf("%s in col %d\n", words[i], j);
        for (k = 0; words[i][k]; k++)
          matches[match + k][j] = words[i][k];
      }

      /* vertical from bottom? */

    }
    for (j = 0; j < 2 * puzzle_size - 1; j++) {
      /* diagonal from top-left? */
      if ((match = contains(diag_down[j], words[i])) != -1) {
        printf("%s in diag_down %d\n", words[i], j);
      }

      /* diagonal from bottom-right? */


      /* diagonal from bottom-left? */
      if ((match = contains(diag_up[j], words[i])) != -1) {
        printf("%s in diag_up %d\n", words[i], j);
      }


      /* diagonal from top-right? */
    }
  }
}

int contains(char *s1, char *s2) {
  /*
  Returns the index of s2 in s1. If s2 is not found in s1, returns -1.
  */
  char *s1_start;
  char *s2_start;

  /* skip over null bytes in s1 and s2 */
  while (!(*s1)) s1++;
  while (!(*s2)) s2++;
 
  /* record start of strings */
  s1_start = s1;
  s2_start = s2;

  /* loop until characters match */
  while (*s1 && *s2) {
    s2 = s2_start;
    if (*s1++ == *s2++) {
      /* continue looping while the characters match */
      while (*s1 && *s2 && (*s1 == *s2)) {
        s1++;
        s2++;
      }
    }
  }

  /* if entire string was matched, return its starting index */
  if (*s2 == '\0')
    return (s1 - s1_start) - (s2 - s2_start);

  /* string was not matched */
  return -1;
}

int contains_reverse(char **s1, char **s2) {
  /*
  Returns the index of s2 in s1 (backwards). If s2 is not found in s1, 
  returns -1
  */
  return -1;
}

/*********************************************************/
/* logging functions                                     */
/*********************************************************/

void emit_matches() {
  /*
  Emits the letters in the puzzle that were matched by one or more words.
  */
  int i;
  int j;

  for (i = 0; i < puzzle_size; i++) {
    for (j = 0; j < puzzle_size; j++) {
      if (matches[i][j])
        printf("%c ", matches[i][j]);
      else
        printf("  ");
    }
    printf("\n");
  }
}

void error_fatal(const char *message) {
  /*
  Display an error message and terminate the program.
  */
  puts(message);
  exit(1);
}

/*********************************************************/
/* main program                                          */
/*********************************************************/

int main(int argc, int **argv) {

  /* read the N x N puzzle */
  read_puzzle(stdin);

  /* read the words */
  read_words(stdin);

  /* build rows, cols, diag_up, and diag_down structures */
  build_structures();

  /* do the searches */
  search_words();
  //char s1[] = "hello2";
  //char s2[] = "hello2";
  //printf("%d\n", contains(s1, s2));

  /* emit the matched words */
  emit_matches();

  return 0;
}
