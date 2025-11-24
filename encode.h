#ifndef ENCODE_H
#define ENCODE_H

#include "circular_buffer.h"
#include "lz77_common.h"
#include <stdio.h>

typedef struct TokenNode {
  Token t;
  struct TokenNode *next;
} TokenNode;


void encode_file(Ahead *a, FILE *f);
void save(Token token, FILE *f);
void read_file(const char *filename, Ahead *a);

#endif
