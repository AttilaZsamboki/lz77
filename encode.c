#include "encode.h"
#include "debugmalloc.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



void save(Token token, FILE *f) {
  long pos_before = ftell(f);
  if (fwrite(&token, sizeof(token), 1, f) != 1) {
    fprintf(stderr, "mentes: fwrite sikertelen\n");
  }
}

void read_file(const char *filename, Ahead *a) {
  if (!a)
    return;

  FILE *f = fopen(filename, "rb");
  if (!f) {
    perror("fajl_olvasas: fopen sikertelen");
    return;
  }

  fseek(f, 0, SEEK_END);
  a->size = ftell(f);
  rewind(f);

  a->buffer = malloc(a->size + 1);
  if (!a->buffer) {
    perror("fajl_olvasas: malloc sikertelen");
    fclose(f);
    return;
  }

  size_t bytes_read = fread(a->buffer, 1, a->size, f);
  if (bytes_read != (size_t)a->size) {
    fprintf(stderr, "fajl_olvasas: fread hiba. Vart: %ld, kapott: %zu\n",
            a->size, bytes_read);
  }

  fclose(f);
}

void init_encoder(Buffer *b) {
  b->size = 0;
  b->end = 0;
  b->start = 0;
  memset(b->data, 0, LENGTH);
}

void free_tokens(TokenNode *head) {
  while (head) {
    TokenNode *temp = head;
    head = head->next;
    free(temp);
  }
}

void append_token(TokenNode **head, TokenNode **tail, Token t) {
  TokenNode *new_node = (TokenNode *)malloc(sizeof(TokenNode));
  if(!new_node) {
    perror("Nem sikerult memoriat foglalni");
    exit(1);
  }
  new_node->t = t;
  new_node->next = NULL;

  if (*tail) {
    (*tail)->next = new_node;
  } else {
    *head = new_node;
  }
  *tail = new_node;
}

void encode_file(Ahead *a, FILE *f) {
  Buffer b;
  init_encoder(&b);

  int idx = 0;
  TokenNode *head = NULL, *tail = NULL;

  while (idx < a->size) {
    Token token;
    Ahead lookahead = {a->size - idx, a->buffer + idx};
    Match match = find_match(&b, &lookahead);

    // Legalább egy karakternek maradnia kell a token.next számára
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
      }
      if (idx < a->size) {
        add(a->buffer[idx++], &b);
      }
    } else {
      token.distance = 0;
      token.length = 0;
      token.next = (unsigned char)(a->buffer[idx]);

      add(a->buffer[idx++], &b);
    }

    append_token(&head, &tail, token);
  }

  TokenNode *current = head;
  while (current) {
    save(current->t, f);
    current = current->next;
  }
  free_tokens(head);
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Hasznalat:\n");
    printf("  %s <bemenet> <kimenet>\n", argv[0]);
    return 1;
  }

  char *input_path = argv[1];
  char *output_path = argv[2];

  Ahead *a = malloc(sizeof(Ahead));
  if (!a) {
    perror("malloc sikertelen");
    return 1;
  }

  a->buffer = NULL;
  read_file(input_path, a);

  if (!a->buffer) {
    fprintf(stderr, "Nem sikerult beolvasni a bemeneti fajlt: '%s'\n", input_path);
    free(a);
    return 1;
  }

  FILE *f = fopen(output_path, "wb");
  if (!f) {
    perror("Fajl megnyitasa sikertelen!");
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
