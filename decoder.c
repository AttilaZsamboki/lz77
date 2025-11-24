#include "decoder.h"
/*#include "debugmalloc.h"*/
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

unsigned char *read_file(const char *filename, size_t *out_size) {
  FILE *f = fopen(filename, "rb");
  if (!f) {
    perror("Hiba a fajl megnyitasakor");
    return NULL;
  }

  size_t capacity = 4096;
  size_t size = 0;
  unsigned char *data = malloc(capacity);
  if (!data) {
    perror("malloc");
    fclose(f);
    return NULL;
  }

  Token token;
  while (fread(&token, sizeof(token), 1, f) == 1) {
    size_t required_size = size + (token.length > 0 ? token.length + 1 : 1);
    
    if (required_size > capacity) {
      size_t new_capacity = capacity * 2;
      if (new_capacity < required_size) new_capacity = required_size + 4096;
      
      unsigned char *new_data = realloc(data, new_capacity);
      if (!new_data) {
        perror("realloc");
        free(data);
        fclose(f);
        return NULL;
      }
      data = new_data;
      capacity = new_capacity;
    }

    if (token.length == 0 && token.distance == 0) {
      data[size++] = token.next;
    } else {
      int start = size - token.distance;
      // Alapvető ellenőrzés érvénytelen tömörített adatokra
      if (start < 0) {
          fprintf(stderr, "Ervenytelen tomoritett adat: a tavolsag tul nagy\n");
          free(data);
          fclose(f);
          return NULL;
      }
      
      for (int i = 0; i < token.length; i++) {
        data[size++] = data[start + i];
      }
      data[size++] = token.next;
    }
  }

  fclose(f);

  if (out_size)
    *out_size = size;
  return data;
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Hasznalat:\n");
    printf("  %s <bemenet> <kimenet>\n", argv[0]);
    return 1;
  }

  size_t size;
  unsigned char *data = read_file(argv[1], &size);
  if (!data)
    return 1;

  FILE *f = fopen(argv[2], "wb");
  if (!f) {
    perror("Hiba a kimeneti fajl megnyitasakor");
    free(data);
    return 1;
  }

  fwrite(data, 1, size, f);
  fclose(f);

  free(data);
  return 0;
}
