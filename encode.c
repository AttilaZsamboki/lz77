#include "encode.h"
#include "debugmalloc.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void pop(char *str) {
  size_t len = strlen(str);
  if (len > 0) {
    memmove(str, str + 1, len);
  }
}

void save(Token token, FILE *f, int token_num) {
  long pos_before = ftell(f);
  if (fwrite(&token, sizeof(token), 1, f) != 1) {
    fprintf(stderr, "save: fwrite failed\n");
  }

  if (token_num >= 95 && token_num <= 102) {
    fprintf(stderr, "  save token %d at offset %ld: d=%d l=%d next=0x%02x\n",
            token_num, pos_before, token.distance, token.length, token.next);
  }
}

void read_file(const char *filename, Ahead *a) {
  if (!a)
    return;

  FILE *f = fopen(filename, "rb");
  if (!f) {
    perror("read_file: fopen failed");
    return;
  }

  fseek(f, 0, SEEK_END);
  a->size = ftell(f);
  rewind(f);

  a->buffer = malloc(a->size + 1);
  if (!a->buffer) {
    perror("read_file: malloc failed");
    fclose(f);
    return;
  }

  size_t bytes_read = fread(a->buffer, 1, a->size, f);
  if (bytes_read != (size_t)a->size) {
    fprintf(stderr, "read_file: fread incomplete. Expected %ld, got %zu\n",
            a->size, bytes_read);
  }

  fclose(f);
}

static void init_encoder(Buffer *b) {
  b->size = 0;
  b->end = 0;
  b->start = 0;
  memset(b->data, 0, LENGTH);
}

void encode_file(Ahead *a, FILE *f) {
  Buffer b;
  init_encoder(&b);

  int idx = 0;
  size_t output_pos = 0;
  int token_count = 0;

  while (idx < a->size) {
    Token token;
    Ahead lookahead = {a->size - idx, a->buffer + idx};
    Match match = find_match(&b, &lookahead);

    // Ensure we have at least one character left for token.next
    if (match.l >= lookahead.size && match.l > 0) {
      match.l = lookahead.size - 1;
    }

    if (match.l > 0) {
      token.distance = (uint16_t)match.d;
      token.length = (uint8_t)match.l;

      int next_idx = idx + match.l;
      token.next = (next_idx < a->size) ? a->buffer[next_idx] : 0;

      for (int i = 0; i < (int)token.length; i++) {
        add(a->buffer[idx++], &b);
        output_pos++;
      }
      if (idx < a->size) {
        add(a->buffer[idx++], &b);
        output_pos++;
      }
    } else {
      token.distance = 0;
      token.length = 0;
      token.next = (unsigned char)(a->buffer[idx]);

      add(a->buffer[idx++], &b);
      output_pos++;
    }

    save(token, f, token_count);
    token_count++;
  }
  fprintf(stderr, "Encoded %d tokens, output %zu bytes\n", token_count,
          output_pos);
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Usage:\n");
    printf("  %s <input> <output>\n", argv[0]);
    return 1;
  }

  char *input_path = argv[1];
  char *output_path = argv[2];

  Ahead *a = malloc(sizeof(Ahead));
  if (!a) {
    perror("malloc failed");
    return 1;
  }

  a->buffer = NULL;
  read_file(input_path, a);

  if (!a->buffer) {
    fprintf(stderr, "Failed to read input file '%s'\n", input_path);
    free(a);
    return 1;
  }

  FILE *f = fopen(output_path, "wb");
  if (!f) {
    perror("File open failed!");
    free(a->buffer);
    free(a);
    return 1;
  }

  encode_file(a, f);

  fclose(f);
  free(a->buffer);
  free(a);
  return 0;
}
