#include "circular_buffer.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void pop(char *str) {
  for (int i = 0; str[i] != '\0'; i++) {
    str[i] = str[i + 1];
  }
}

typedef struct Token {
  uint16_t distance;
  uint8_t length;
  unsigned char next;
} Token;

void save(Token token, FILE *f) { fwrite(&token, sizeof(token), 1, f); }

char *read_file(const char *filename) {
  FILE *f = fopen(filename, "rb"); // use "rb" for binary or text
  if (!f)
    return NULL;

  fseek(f, 0, SEEK_END);
  long size = ftell(f);
  rewind(f);

  char *buffer = malloc(size + 1);
  if (!buffer)
    return NULL;

  fread(buffer, 1, size, f);
  buffer[size] = '\0';

  fclose(f);
  return buffer;
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Usage:\n");
    printf("  %s encode <input> <output>\n", argv[0]);
    printf("  %s decode <input> <output>\n", argv[0]);
    return 1;
  }

  char *output = argv[3];

  char *search[LENGTH] = {NULL};

  char *ahead = read_file(argv[1]);

  FILE *f = fopen(argv[2], "ab");
  if (!f) {
    perror("File open failed!");
    return 1;
  }
  while (ahead[0] != '\0') {
    struct Token token;
    struct Match match = find_match(search, ahead);
    if (match.l) {
      token.distance = match.d;
      token.length = match.l - 1;
      token.next = match.c;
    } else {
      token.distance = 0;
      token.length = 0;
      token.next = ahead[0];
    }
    for (int i = 0; i < token.length + 1; i++) {
      add(ahead[0], search);
      pop(ahead);
    }
    save(token, f);
  }
  fclose(f);
  return 0;
}
