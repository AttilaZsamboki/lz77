#ifndef LZ77_COMMON_H
#define LZ77_COMMON_H

#include <stdint.h>

typedef struct Token
{
  uint16_t distance;
  uint8_t length;
  unsigned char next;
} Token;

#endif
