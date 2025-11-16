#include "circular_buffer.h"
#include "debugmalloc.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void pop(char *str)
{
  for (int i = 0; str[i] != '\0'; i++)
  {
    str[i] = str[i + 1];
  }
}

typedef struct Token
{
  uint16_t distance;
  uint8_t length;
  unsigned char next;
} Token;

void save(Token token, FILE *f, int token_num) { 
  long pos_before = ftell(f);
  if (fwrite(&token, sizeof(token), 1, f) != 1) {
    fprintf(stderr, "save: fwrite failed\n");
  }
  long pos_after = ftell(f);
  
  if (token_num >= 95 && token_num <= 102) {
    fprintf(stderr, "  save token %d at offset %ld: d=%d l=%d next=0x%02x\n",
            token_num, pos_before, token.distance, token.length, token.next);
  }
}

void read_file(const char *filename, Ahead *a)
{
  FILE *f = fopen(filename, "rb");
  if (!f)
  {
    perror("read_file: fopen failed");
    return;
  }

  fseek(f, 0, SEEK_END);
  a->size = ftell(f);
  rewind(f);

  a->buffer = malloc(a->size + 1);
  if (!a->buffer)
  {
    perror("read_file: malloc failed");
    fclose(f);
    return;
  }

  size_t bytes_read = fread(a->buffer, 1, a->size, f);
  if (bytes_read != (size_t)a->size)
  {
    fprintf(stderr, "read_file: fread incomplete. Expected %ld, got %zu\n", a->size, bytes_read);
  }

  fclose(f);
}

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    printf("Usage:\n");
    printf("  %s <input> <output>\n", argv[0]);
    return 1;
  }

  char *input_path = argv[1];
  char *output_path = argv[2];

  Buffer b = {0, 0, 0, {0}};

  Ahead *a = malloc(sizeof(Ahead));
  read_file(input_path, a);
  if (!a)
  {
    fprintf(stderr, "Failed to read input file '%s'\n", input_path);
    return 1;
  }

  FILE *f = fopen(output_path, "wb");
  if (!f)
  {
    perror("File open failed!");
    return 1;
  }
  int idx = 0;
  size_t output_pos = 0; /* Track logical output position */
  int token_count = 0;
  while (a->size > idx)
  {
    Token token;
    /* Create a temporary Ahead struct pointing to the remaining lookahead */
    Ahead lookahead = {a->size - idx, a->buffer + idx};
    Match match = find_match(&b, &lookahead);
    if (match.l > 0)
    {
      token.distance = (uint16_t)match.d;
      token.length = (uint8_t)match.l;
      /* Peek ahead to see if there's a next byte AFTER the match */
      int next_idx = idx + match.l;
      token.next = (next_idx < a->size) ? a->buffer[next_idx] : 0;
    }
    else
    {
      token.distance = 0;
      token.length = 0;
      token.next = (unsigned char)(a->buffer[idx]);
    }
    for (int i = 0; i < (int)token.length + 1; i++)
    {
      add(a->buffer[idx++], &b);
      output_pos++;
    }
    /* The next byte is already included in the token.length loop above */
    if (idx < a->size)
    {
      add(a->buffer[idx], &b);
      idx++;
      output_pos++;
    }
    
    if (token_count >= 0 && token_count <= 2)
    {
      fprintf(stderr, "Token %d: d=%d l=%d next=0x%02X output_pos=%zu idx=%d\n", 
              token_count, token.distance, token.length, token.next, output_pos, idx);
    }
    
    save(token, f, token_count);
    token_count++;
    
    if (token_count <= 3)
    {
      fprintf(stderr, "Token %d hex: %02x %02x %02x %02x\n",
              token_count - 1,
              ((unsigned char*)&token)[0],
              ((unsigned char*)&token)[1],
              ((unsigned char*)&token)[2],
              ((unsigned char*)&token)[3]);
    }
  }
  fprintf(stderr, "Encoded %d tokens, output %zu bytes\n", token_count, output_pos);
  fclose(f);
  free(a->buffer);
  free(a);
  return 0;
}
