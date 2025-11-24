#include "circular_buffer.h"
#include "debugmalloc.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

void add(char item, Buffer *b) {
  b->data[b->end] = (unsigned char)item;
  b->end = (b->end + 1) % LENGTH;
  if (b->size == LENGTH) {
    /* buffer tele: legrégebbi elem felülírása */
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
      // Eltérés. Ha volt találat, nézzük meg, hogy ez a leghoszabb-e eddig.
      if (subend > end) {
        end = subend;
        // Relatív távolság kiszámolása
        int dist = (b->end - subd + LENGTH) % LENGTH;
        if (dist == 0)
          dist = LENGTH;
        d = dist;
        c = subend == a->size ? 0 : a->buffer[subend]; // Kövi karakter
      }

      subend = 0;
      subd = -1;
      // Új találat kezdése, ha egyezést talál
      if (a->buffer[0] == b->data[j]) {
        subd = j;
        subend = 1;
      }
    }
  }

  // A ciklus végének ellenőrzése (bent maradt-e egy találat ami nem volt
  // kezelve)
  if (subend > end) {
    end = subend;
    int dist = (b->end - subd + LENGTH) % LENGTH;
    if (dist == 0)
      dist = LENGTH;
    d = dist;
    c = subend == a->size ? 0 : a->buffer[subend];
  }

  // Ha nincs találat annak kezelése
  if (end == 0) {
    d = 0;
    c = a->buffer[0];
  }

  struct Match match2 = {d, end, c};
  return match2;
}
