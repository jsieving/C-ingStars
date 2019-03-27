#include <stdio.h>
#include <stdlib.h>
#include "my_malloc.h"

int main() {
  int sz = 0;
  int space = 0;
  void* p;

  while (space < 200) {
    sz = 32 + (rand() % 61);
    space += sz;
    p = my_malloc(sz);
    printf("\nNew block @ %p of %d bytes.\n", p, sz);
    puts("Running traverse_blocks...");
    traverse_blocks();
  }

  puts("---------------------------------------\n---------------------------------------");

  struct block_meta* current = global_base;
  while (current->next->next) {
    current = current->next;
  }

  while (current) {
    printf("\nFreeing block @ %p|%p.\n", current, current+1);
    my_free(current+1);
    puts("Running traverse_blocks...");
    traverse_blocks();
    current = current->prev;
  }

  space = 0;

  puts("++++++++++++++++++++++++++++++++++++++++\n++++++++++++++++++++++++++++++++++++++++");

  while (space < 200) {
    sz = 32 + (random() % 32);
    space += sz;
    p = my_malloc(sz);
    printf("\nNew block @ %p of %d bytes.\n", p, sz);
    puts("Running traverse_blocks...");
    traverse_blocks();
  }
  return 0;
}
