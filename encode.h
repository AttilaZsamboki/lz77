#ifndef ENCODE_H
#define ENCODE_H

#include "lz77_common.h"
#include "circular_buffer.h"
#include <stdio.h>

void pop(char *str);
void save(Token token, FILE *f, int token_num);
void read_file(const char *filename, Ahead *a);

#endif
