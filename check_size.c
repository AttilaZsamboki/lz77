#include <stdint.h>
#include <stdio.h>

typedef struct Token
{
  uint16_t distance;
  uint8_t length;
  unsigned char next;
} Token;

int main() { 
  printf("sizeof(Token)=%zu\n", sizeof(Token));
  printf("sizeof(uint16_t)=%zu\n", sizeof(uint16_t));
  printf("sizeof(uint8_t)=%zu\n", sizeof(uint8_t));
  printf("sizeof(unsigned char)=%zu\n", sizeof(unsigned char));
  printf("offsetof distance=0\n");
  printf("offsetof length=%zu\n", offsetof(Token, length));
  printf("offsetof next=%zu\n", offsetof(Token, next));
  return 0;
}
