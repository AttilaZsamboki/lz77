#ifndef DECODER_H
#define DECODER_H

#include "lz77_common.h"
#include <stddef.h>

unsigned char *read_file(const char *filename, size_t *out_size);

#endif
