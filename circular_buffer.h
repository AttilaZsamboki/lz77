#include <stdbool.h>

#ifndef CIRCULAR_BUFFER
#define CIRCULAR_BUFFER

#define LENGTH 4096

typedef struct Match {
  int d, l;
  char c;
} Match;

void add(char item, char *circular_buffer[LENGTH]);
void remove_item(char *circular_buffer[LENGTH]);
void print_buffer(char *circular_buffer[LENGTH]);
struct Match find_match(char *circular_buffer[LENGTH], char input[50]);

#endif
