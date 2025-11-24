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
  int end = 0, subend = 0, subd = -1, d = 0;
  char c = a->buffer[0];
  
  for (int j = 0; subend < a->size && j < b->size; j++) {

    if (a->buffer[subend] == b->data[j]) {
      if (subd == -1) {
        subd = j;
      }
      subend++;
    } else {
      // Mismatch. If we were tracking a match, check if it's the best so far.
      if (subend > end) {
        end = subend;
        // Calculate distance relative to current end
        int dist = (b->end - subd + LENGTH) % LENGTH;
        if (dist == 0) dist = LENGTH;
        d = dist;
        c = subend == a->size ? 0 : a->buffer[subend]; // Next char after match
      }
      
      // Reset to search for new match
      // But wait, we might have missed a match starting at j if we were in the middle of another match.
      // This simple greedy search is flawed if we just reset.
      // However, for a simple LZ77, maybe we just reset subend and subd.
      // But we need to continue searching from the current j? 
      // If we mismatch at j, maybe j could be the start of a new match?
      // The original code was also weird.
      
      // Let's try to implement a simpler search or fix the existing one.
      // The original code:
      /*
      if (a->buffer[0] == b->data[j] && !subd) { subd = j; }
      if (a->buffer[subend] == b->data[j]) { subend++; }
      if ((mismatch) && (subend || !end)) { ... }
      */
      
      // To properly find the longest match, we should probably look for all occurrences of a->buffer[0]
      // and extend them. But that's O(N*M).
      // The current loop is O(N). It tries to find *a* match.
      
      // Let's stick to the structure but fix the bugs.
      
      subend = 0;
      subd = -1;
      // If the current char matches start of lookahead, start a new match
      if (a->buffer[0] == b->data[j]) {
        subd = j;
        subend = 1;
      }
    }
  }
  
  // Check at the end of loop
  if (subend > end) {
    end = subend;
    int dist = (b->end - subd + LENGTH) % LENGTH;
    if (dist == 0) dist = LENGTH;
    d = dist;
    c = subend == a->size ? 0 : a->buffer[subend];
  }
  
  // If no match found (end == 0), return literal
  if (end == 0) {
      d = 0;
      c = a->buffer[0];
  }

  struct Match match2 = {d, end, c};
  return match2;
}
