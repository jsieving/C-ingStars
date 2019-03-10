#include <stdio.h>

struct block_meta {
  size_t size;
  struct block_meta *next;
  int free;
};
struct block_meta *find_free_block(struct block_meta **last, size_t size);
struct block_meta *request_space(struct block_meta *last, size_t size);
void *my_malloc(size_t size);
struct block_meta *get_block_ptr(void *ptr);
void my_free(void*ptr);
void *my_realloc(void *ptr, size_t size);
void *my_calloc(size_t nelem, size_t elsize);
