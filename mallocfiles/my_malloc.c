#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include "my_malloc.h"

void* global_base = NULL; // head of linked list of memory blocks


/* find_free_block: iterates through linked list looking
  for my_free block of correct size

  returns: my_free block
*/
Block *find_free_block(Block **last, size_t size) {
  Block *current = global_base;
  while (current && !(current->free && current->size >= size)) {
    *last = current;
    current = current->next;
  }
  return current;
}

/* request_space: use sbrk to allocate space and add new block to linked list
  last: last block of linked list
  size: size of space to allocate

  returns: newly allocated block
*/
Block *request_space(Block* last, size_t size) {
  Block *block;
  block = sbrk(0); // points to current top of heap
  void *request = sbrk(size + META_SIZE);
  assert((void*)block == request); // TODO: Not thread safe.
  if (request == (void*) -1) {
    return NULL; // sbrk failed.
  }

  if (last) { // NULL on first request, so add to linked list.
    last->next = block;
  }
  block->prev = last;
  block->size = size;
  block->next = NULL;
  block->free = 0;
  block->magic = 0x12345678;
  return block;
}

/* my_malloc: allocates space on heap using sbrk

  size_t: number of bytes to allocate

  returns: pointer to allocated space
*/
void *my_malloc(size_t size) {
  Block *block;
  // TODO: align size

  if (size <= 0) {
    return NULL;
  }

  if (!global_base) { // on first call, try to create global base
    block = request_space(NULL, size);
    if (!block) {  // if request space failed
      return NULL;
    }
    global_base = block;
  } else {
    Block *last = global_base;
    block = find_free_block(&last, size);
    if (!block) { // Failed to find free block.
      block = request_space(last, size);
      if (!block) {
        return NULL;
      }
    } else {      // Found free block
      if (block->size > size + META_SIZE) { // If block is larger than necessary
        Block *leftover;
        long int first_pos = (long int) block;
        leftover = (Block *) (first_pos + size + META_SIZE);
        leftover->size = block->size - size - META_SIZE;
        leftover->next = block->next;
        leftover->prev = block;
        leftover->free = 1;
        leftover->magic = 0x22222222;
        block->next = leftover;
        block->size = size;
      }
      block->free = 0;
      block->magic = 0x77777777;
    }
  }

  return(block + 1);
  // block+1 because want to point to the region after block_meta
  // and +1 increments by one sizeof(struct(block_meta))
}

Block *get_block_ptr(void *ptr) {
  return (Block*)ptr - 1;
}

// my_free: my_frees block at given pointer
void my_free(void *ptr) {
  if (!ptr) {
    return;
  }
  Block* block_ptr = get_block_ptr(ptr);
  assert(block_ptr->magic == 0x77777777 || block_ptr->magic == 0x12345678);
  assert(block_ptr->free == 0);
  // If this is the last block, just cut it off
  if (block_ptr->next == NULL && block_ptr->prev) {
    block_ptr->prev->next = NULL;
    return;
  }
  // If the next block is free, absorb its space
  if (block_ptr->next && block_ptr->next->free == 1) {
    block_ptr->size += META_SIZE + block_ptr->next->size;
    block_ptr->next = block_ptr->next->next;
    if (block_ptr->next) {block_ptr->next->prev = block_ptr->prev;}
  }
  // If the previous block is free, grow it and tell it to jump over this one
  if (block_ptr->prev && block_ptr->prev->free == 1) {
    block_ptr->prev->size += META_SIZE + block_ptr->size;
    block_ptr->prev->next = block_ptr->next;
    if (block_ptr->next) {block_ptr->next->prev = block_ptr->prev;}
  }
  // Set the free flag to 1 (doesn't do much if prev is free or gets cut off)
  block_ptr->free = 1;
}

void *my_realloc(void *ptr, size_t size) {
  if (!ptr) {
    // NULL ptr. realloc should act like malloc.
    return my_malloc(size);
  }

  Block* block_ptr = get_block_ptr(ptr);
  if (block_ptr->size >= size) {
    // We have enough space. Could free some once we implement split.
    return ptr;
  }

  // Need to really realloc. Malloc new space and free old space.
  // Then copy old data to new space.
  void *new_ptr;
  new_ptr = my_malloc(size);
  if (!new_ptr) {
    return NULL; // TODO: set errno on failure.
  }
  memcpy(new_ptr, ptr, block_ptr->size);
  my_free(ptr);
  return new_ptr;
}

void *my_calloc(size_t nelem, size_t elsize) {
  size_t size = nelem * elsize; // TODO: check for overflow.
  void *ptr = my_malloc(size);
  memset(ptr, 0, size);
  return ptr;
}

void traverse_blocks() {
  Block *current = global_base;
  int i = 0;
  printf("{ global_base: %p\n", global_base);
  while (current) {
    printf("\t%d: [%p|%p] free = %d, size = %li, magic = %x\n", i, current, current+1, current->free, current->size, current->magic);
    if (current->next != NULL) {
      printf("|%li| %li |\n", (long int) current->next - (long int) current, (long int) current->next - (long int) current - 32);
    }
    current = current->next;
    i++;
  }
  puts("}\n");
}
