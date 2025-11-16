#include <stdbool.h>

#ifndef CIRCULAR_BUFFER
#define CIRCULAR_BUFFER

#define LENGTH 4096

typedef struct Match {
  int d, l;
  char c;
} Match;

typedef struct Buffer {
  int size, end, start;
  unsigned char data[LENGTH];
} Buffer;

typedef struct Ahead {
  long size;
  unsigned char *buffer;
} Ahead;

void add(char item, Buffer *b);
void print_buffer(Buffer *b);
struct Match find_match(Buffer *b, Ahead *a);

#endif
