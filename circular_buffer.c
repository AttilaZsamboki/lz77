#include "circular_buffer.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int end, start;

void add(char item, char *circular_buffer[LENGTH]) {
  char *p = malloc(sizeof(char));
  *p = item;
  circular_buffer[end] = p;

  if (end == LENGTH - 1) {
    end = 0;
  } else {
    end++;
  }

  if (circular_buffer[end]) {
    start = end;
  }
}

void remove_item(char *circular_buffer[LENGTH]) {
  circular_buffer[start] = NULL;
  if (start == LENGTH) {
    start = 0;
    return;
  }
  start++;
}

void print_buffer(char *circular_buffer[LENGTH]) {
  for (int i = 0; i < LENGTH; i++) {
    if (!circular_buffer[i]) {
      printf("-");
      continue;
    }
    printf("%c", *circular_buffer[i]);
  }
  printf("\n");
}

int get_length(char *search[LENGTH]) {
  int i;
  for (i = 0; i <= LENGTH && search[i] != NULL; i++);
  return i;
}

Match find_match(char *search[LENGTH], char ahead[50]) {
  int end = 0, subend = 0, subd = 0, d;
  char c;
  int length = get_length(search);
  for (int j = 0; ahead[subend] != '\0' && search[j] != NULL; j++) {

    if (ahead[0] == *search[j] && !subd) {
      subd = j;
    }

    if (ahead[subend] == *search[j]) {
      subend++;
    } 

    if ((ahead[subend] != *search[j] || ahead[subend] == '\0' || search[j+1] == NULL) && (subend || !end) ) { 
      if (subend > end) {
        end = subend;
        d = length - subd;
        c = ahead[subend] == '\0' ? '\0' : ahead[subend-1];
      }
      subend = 0;
      subd = 0;
    }
  }
  struct Match match2 = {d, end, c};
  return match2;
}
