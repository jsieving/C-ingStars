#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

typedef struct block_meta {
  size_t size;
  struct block_meta *prev;
  struct block_meta *next;
  int free;
} Block;

#define META_SIZE sizeof(struct block_meta)

extern void *global_base;

struct block_meta *find_free_block(struct block_meta **last, size_t size);
struct block_meta *request_space(struct block_meta* last, size_t size);
void *my_malloc(size_t size);
struct block_meta *get_block_ptr(void *ptr);
void my_free(void *ptr);
void *my_realloc(void *ptr, size_t size);
void *my_calloc(size_t nelem, size_t elsize);
void traverse_blocks();
