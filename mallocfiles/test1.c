#include <stdio.h>
#include <stdlib.h>
#include "my_malloc.h"

int main() {
  int sz = 0;
  void* p;
  printf("sizeof(block_meta) = %li\n", sizeof(struct block_meta));

  while (sz < 130) {
    p = my_malloc(sz);
    printf("\nNew block @ %p.\n", p);
    puts("Running traverse_blocks...");
    traverse_blocks();
    sz += 32;
  }

  struct block_meta* current = global_base;
  while (current) {
    printf("\nFreeing block @ %p|%p.\n", current, current+1);
    my_free(current+1);
    puts("Running traverse_blocks...");
    traverse_blocks();
    current = current->next;
  }
  return 1;
}
