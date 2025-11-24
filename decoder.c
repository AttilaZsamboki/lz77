#include "decoder.h"
/*#include "debugmalloc.h"*/
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

unsigned char *read_file(const char *filename, size_t *out_size) {
  FILE *f = fopen(filename, "rb");
  if (!f) {
    perror("Error opening file");
    return NULL;
  }

  fseek(f, 0, SEEK_END);
  long file_size = ftell(f);
  rewind(f);

  size_t max_output =
      (file_size < 1000000) ? (file_size * 100 + 4096) : 104857600;
  unsigned char *data = malloc(max_output);
  if (!data) {
    perror("malloc");
    fclose(f);
    return NULL;
  }
  Token token;
  int end = 0;
  while (fread(&token, sizeof(token), 1, f) == 1) {
    if (token.length == 0 && token.distance == 0) {
      data[end] = token.next;
      end++;
    } else {
      int start = end - token.distance;
      for (int i = 0; i < token.length; i++) {
        data[end] = data[i + start];
        end++;
      }
      data[end] = token.next;
      end++;
    }
  }
  fclose(f);

  if (out_size)
    *out_size = end;
  return data;
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Usage:\n");
    printf("  %s <input> <output>\n", argv[0]);
    return 1;
  }

  size_t size;
  unsigned char *data = read_file(argv[1], &size);
  if (!data)
    return 1;

  FILE *f = fopen(argv[2], "wb");
  if (!f) {
    perror("Error opening output file");
    free(data);
    return 1;
  }

  fwrite(data, 1, size, f);
  fclose(f);

  free(data);
  return 0;
}
