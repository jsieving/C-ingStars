#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

struct block_meta {
  size_t size;
  struct block_meta *next;
  int free;
  int magic; // For debugging only. TODO: remove this in non-debug mode.
};

#define META_SIZE sizeof(struct block_meta)

extern void *global_base;

struct block_meta *find_free_block(struct block_meta **last, size_t size);
struct block_meta *request_space(struct block_meta* last, size_t size);
void *jsmalloc(size_t size);
struct block_meta *get_block_ptr(void *ptr);
void jsfree(void *ptr);
void *jsrealloc(void *ptr, size_t size);
void *jscalloc(size_t nelem, size_t elsize);
void traverse_blocks();
