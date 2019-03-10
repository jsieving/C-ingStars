#include <stdio.h>
#include <stdlib.h>
#include "jsmalloc.h"

void main() {
  int sz = 0;
  void* p;
  printf("sizeof(block_meta) = %li\n", sizeof(struct block_meta));

  while (sz < 130) {
    p = jsmalloc(sz);
    printf("\nNew block @ %p.\n", p);
    puts("Running traverse_blocks...");
    traverse_blocks();
    sz += 32;
  }

  struct block_meta* current = global_base;
  while (current) {
    printf("\nFreeing block @ %p|%p.\n", current, current+1);
    jsfree(current+1);
    puts("Running traverse_blocks...");
    traverse_blocks();
    current = current->next;
  }
}
