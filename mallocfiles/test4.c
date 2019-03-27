#include <stdio.h>
#include <stdlib.h>
#include "my_malloc.h"

int main() {
  void* p;
  void* q;
  void* r;
  void* s;
  void* t;

  p = my_malloc(36);
  printf("\nNew block @ %p of %d bytes.\n", p, 36);
  puts("Running traverse_blocks...");
  traverse_blocks();

  q = my_malloc(72);
  printf("\nNew block @ %p of %d bytes.\n", q, 72);
  puts("Running traverse_blocks...");
  traverse_blocks();

  r = my_malloc(50);
  printf("\nNew block @ %p of %d bytes.\n", r, 50);
  puts("Running traverse_blocks...");
  traverse_blocks();

  s = my_malloc(25);
  printf("\nNew block @ %p of %d bytes.\n", s, 25);
  puts("Running traverse_blocks...");
  traverse_blocks();

  r = my_realloc(r, 75);
  printf("\nRealloc'd r block @ %p to size 75.\n", r);
  puts("Running traverse_blocks...");
  traverse_blocks();

  q = my_realloc(q, 110);
  printf("\nRealloc'd q block @ %p to size 110.\n", q);
  puts("Running traverse_blocks...");
  traverse_blocks();

  t = my_calloc(20, 4);
  printf("\nCalloc'd block @ %p with 20 * 4 bytes.\n", t);
  puts("Running traverse_blocks...");
  traverse_blocks();

  printf("\nFreeing block @ %p.\n", p);
  my_free(p);
  puts("Running traverse_blocks...");
  traverse_blocks();

  printf("\nFreeing block @ %p.\n", q);
  my_free(q);
  puts("Running traverse_blocks...");
  traverse_blocks();

  printf("\nFreeing block @ %p.\n", r);
  my_free(r);
  puts("Running traverse_blocks...");
  traverse_blocks();

  printf("\nFreeing block @ %p.\n", s);
  my_free(s);
  puts("Running traverse_blocks...");
  traverse_blocks();

  printf("\nFreeing block @ %p.\n", t);
  my_free(t);
  puts("Running traverse_blocks...");
  traverse_blocks();

  return 0;
}
