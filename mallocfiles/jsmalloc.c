#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include "jsmalloc.h"

/* Moved to jsmalloc.h:
struct block_meta {
  size_t size;
  struct block_meta *next;
  int jsfree;
  int magic; // For debugging only.
};

#define META_SIZE sizeof(struct block_meta)

void *global_base = NULL;
*/

struct block_meta *find_free_block(struct block_meta **last, size_t size) {
  struct block_meta *current = global_base;
  while (current && !(current->free && current->size >= size)) {
    *last = current;
    current = current->next;
  }
  return current;
}

struct block_meta *request_space(struct block_meta* last, size_t size) {
  struct block_meta *block;
  block = sbrk(0);
  void *request = sbrk(size + META_SIZE);
  assert((void*)block == request); // Not thread safe.
  if (request == (void*) -1) {
    return NULL; // sbrk failed.
  }

  if (last) { // NULL on first request.
    last->next = block;
  }
  block->size = size;
  block->next = NULL;
  block->free = 0;
  block->magic = 0x12345678;
  return block;
}

void *jsmalloc(size_t size) {
  struct block_meta *block;
  // TODO: align size?

  if (size <= 0) {
    return NULL;
  }

  if (!global_base) { // First call.
    block = request_space(NULL, size);
    if (!block) {
      return NULL;
    }
    global_base = block;
  } else {
    struct block_meta *last = global_base;
    block = find_free_block(&last, size);
    if (!block) { // Failed to find jsfree block.
      block = request_space(last, size);
      if (!block) {
        return NULL;
      }
    } else {      // Found jsfree block
      // TODO: consider splitting block here.
      block->free = 0;
      block->magic = 0x77777777;
    }
  }

  return(block+1);
}

struct block_meta *get_block_ptr(void *ptr) {
  return (struct block_meta*)ptr - 1;
}

void jsfree(void *ptr) {
  if (!ptr) {
    return;
  }

  // TODO: consider merging blocks once splitting blocks is implemented.
  struct block_meta* block_ptr = get_block_ptr(ptr);
  assert(block_ptr->magic == 0x77777777 || block_ptr->magic == 0x12345678);
  assert(block_ptr->free == 0);
  block_ptr->free = 1;
  block_ptr->magic = 0x55555555;
}

void *jsrealloc(void *ptr, size_t size) {
  if (!ptr) {
    // NULL ptr. jsrealloc should act like jsmalloc.
    return jsmalloc(size);
  }

  struct block_meta* block_ptr = get_block_ptr(ptr);
  if (block_ptr->size >= size) {
    // We have enough space. Could jsfree some once we implement split.
    return ptr;
  }

  // Need to really jsrealloc. Malloc new space and jsfree old space.
  // Then copy old data to new space.
  void *new_ptr;
  new_ptr = jsmalloc(size);
  if (!new_ptr) {
    return NULL; // TODO: set errno on failure.
  }
  memcpy(new_ptr, ptr, block_ptr->size);
  jsfree(ptr);
  return new_ptr;
}

void *jscalloc(size_t nelem, size_t elsize) {
  size_t size = nelem * elsize; // TODO: check for overflow.
  void *ptr = jsmalloc(size);
  memset(ptr, 0, size);
  return ptr;
}

void traverse_blocks() {
  struct block_meta *current = global_base;
  int i = 0;
  while (current) {
    printf("%d: [%p] free = %d, magic = %x\n", i, &current, current->free, current->magic);
    current = current->next;
  }
}
