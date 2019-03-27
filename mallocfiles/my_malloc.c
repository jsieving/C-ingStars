#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include "my_malloc.h"

void* global_base = NULL; // head of linked list of memory blocks

pthread_mutex_t global_mutex = PTHREAD_MUTEX_INITIALIZER;

/* find_free_block: iterates through linked list looking
  for my_free block of correct size

  returns: my_free block
*/
Block *find_free_block(Block **last, size_t size) {
  pthread_mutex_lock(&global_mutex);
  Block *current = global_base;
  pthread_mutex_unlock(&global_mutex);
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
  return block;
}

/* handle_extra_space: called when a block may have more space than necessary.
  If there is enough extra space in the block to create another block meta, one is created and freed.

  block_ptr: the block which may need size adjustment
  need_size: number of bytes that the block needs to retain, expected to be divisible by 8
*/
void handle_extra_space(Block* block_ptr, size_t need_size) {
  // quit if there is not enough space to create a new Block
  if (block_ptr->size < need_size + META_SIZE) {
    return;
  }
  Block *leftover;
  long int first_pos = (long int) block_ptr;
  leftover = (Block *) (first_pos + META_SIZE + need_size);
  leftover->size = block_ptr->size - need_size - META_SIZE;
  leftover->next = block_ptr->next;
  leftover->prev = block_ptr;
  leftover->free = 0; // free() expects this to be 0
  if (block_ptr->next) block_ptr->next->prev = leftover;
  block_ptr->next = leftover;
  block_ptr->size = need_size;
  my_free(leftover+1); // calling free() makes sure the extra free space
                       // is combined with other free space. +1 is because
                       // it takes a pointer to the data, not metadata.
}

/* my_malloc: allocates space on heap using sbrk

  size_t: number of bytes to allocate

  returns: pointer to allocated space
*/
void *my_malloc(size_t size) {
  Block *block;

  if (size <= 0) {
    return NULL;
  }

  if (size & 0b111) {
    size = ((size >> 3) << 3) + 8; // Round size up to multiple of 8
  }

  if (!global_base) { // on first call, try to create global base
    block = request_space(NULL, size);
    if (!block) {  // if request space failed
      return NULL;
    }
    pthread_mutex_lock(&global_mutex);
    global_base = block;
    pthread_mutex_unlock(&global_mutex);
  } else {
    Block *last = global_base;
    block = find_free_block(&last, size);
    if (!block) { // Failed to find free block.
      block = request_space(last, size);
      if (!block) {
        return NULL;
      }
    } else {      // Found free block
      block->free = 0;
      handle_extra_space(block, size);
    }
  }

  return(block + 1);
  // block+1 because want to point to the region after block_meta
  // and +1 increments by one sizeof(struct(block_meta))
}

Block *get_block_ptr(void *ptr) {
  return (Block*)ptr - 1;
}

// my_free: frees block at given pointer
void my_free(void *ptr) {
  if (!ptr) {
    return;
  }
  Block* block_ptr = get_block_ptr(ptr);
  if (block_ptr->free != 0) puts("Error in my_free.\n");
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
  // Set the free flag to 1 (doesn't do much if prev is free)
  block_ptr->free = 1;
}

void *my_realloc(void *ptr, size_t size) {
  if (!ptr) {
    // NULL ptr. realloc should act like malloc.
    return my_malloc(size);
  }

  if (size & 0b111) {
    size = ((size >> 3) << 3) + 8; // Round size up to multiple of 8
  }

  Block* block_ptr = get_block_ptr(ptr);

  if (block_ptr->size >= size) {
    // We have enough space.
    handle_extra_space(block_ptr, size); // if the block has sufficient excess space, make it into a free block
    return ptr;
  }

  if (block_ptr->next && block_ptr->next->free) { // Look for space in next block if it exists and is free
    Block* next_ptr = block_ptr->next;
    if (next_ptr->size + META_SIZE >= size - block_ptr->size) { // If the next block takes up enough space to grow this block
      block_ptr->size += META_SIZE + next_ptr->size; // Take its home
      block_ptr->next = next_ptr->next; // Take its children
      next_ptr->next->prev = block_ptr; // Convince its children that you are their mother
      handle_extra_space(block_ptr, size); // If absorbing that space yields more than necessary, split it
      return block_ptr+1;
    }
  }

  if (block_ptr->next == NULL) { // expand the block if it is on the end
    void *request = sbrk(size - block_ptr->size);
    if (request == (void*) -1) {
      return NULL; // sbrk failed. TODO: set errno on failure.
    } else {
      block_ptr->size = size;
      return block_ptr+1;
    }
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

  if (size & 0b111) {
    size = ((size >> 3) << 3) + 8; // Round size up to multiple of 8
  }

  void *ptr = my_malloc(size);
  memset(ptr, 0, size);
  return ptr;
}

void traverse_blocks() {
  pthread_mutex_lock(&global_mutex);
  Block *current = global_base;
  pthread_mutex_unlock(&global_mutex);

  int i = 0;
  printf("{ global_base: %p\n", global_base);
  while (current) {
    printf("\t%d: [%p|%p] free = %d, size = %li\n", i, current, current+1, current->free, current->size);
    if (current->next != NULL) {
      printf("|%li| %li |\n", (long int) current->next - (long int) current, (long int) current->next - (long int) current - 32);
    }
    current = current->next;
    i++;
  }
  puts("}\n");
}
