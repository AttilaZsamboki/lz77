#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Token {
  uint16_t distance;
  uint8_t length;
  unsigned char next;
} Token;

char *read_file(const char *filename, size_t *out_size) {
  FILE *f = fopen(filename, "rb");
  if (!f) {
    perror("Error opening file");
    return NULL;
  }

  fseek(f, 0, SEEK_END);
  long size = ftell(f);
  rewind(f);

  char *data = malloc(size * 10);
  if (!data) {
    perror("malloc");
    fclose(f);
    return NULL;
  }
  Token token;
  int end = 0;
  while (fread(&token, sizeof(token), 1, f) == 1) {
    if (token.length == 0 && token.length == 0) {
      data[end] = token.next;
      end++;
    } else {
      int start = end - token.distance;
      for (int i = 0; i <= token.length; i++) {
        data[end] = data[i + start];
        end++;
      }
    }
  }
  fclose(f);

  if (out_size)
    *out_size = end;
  data[end] = '\0';
  return data;
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Usage:\n");
    printf("  %s encode <input> <output>\n", argv[0]);
    printf("  %s decode <input> <output>\n", argv[0]);
    return 1;
  }

  size_t size;
  char *data = read_file(argv[1], &size);
  if (!data)
    return 1;
  FILE *f = fopen(argv[2], "w");
  fprintf(f, "%s", data);

  printf("\n");
  free(data);
  return 0;
}
