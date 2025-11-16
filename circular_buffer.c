#include "circular_buffer.h"
#include "debugmalloc.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

void add(char item, Buffer *b) {
  b->data[b->end] = (unsigned char)item;
  b->end = (b->end + 1) % LENGTH;
  if (b->size == LENGTH) {
    /* buffer full: overwrite oldest element */
    b->start = (b->start + 1) % LENGTH;
  } else {
    b->size++;
  }
}

Match find_match(Buffer *b, Ahead *a) {
  int end = 0, subend = 0, subd = 0, d;
  char c;
  for (int j = 0; subend < a->size && j < b->size; j++) {

    if (a->buffer[0] == b->data[j] && !subd) {
      subd = j;
    }

    if (a->buffer[subend] == b->data[j]) {
      subend++;
    }

    if ((a->buffer[subend] != b->data[j] || subend < a->size ||
         j + 1 == b->size) &&
        (subend || !end)) {
      if (subend > end) {
        end = subend;
        d = b->size - subd;
        c = subend == a->size ? 0 : a->buffer[subend - 1];
      }
      subend = 0;
      subd = 0;
    }
  }
  struct Match match2 = {d, end, c};
  return match2;
}
