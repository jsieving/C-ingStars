#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define META_SIZE sizeof(struct block_meta)

void *global_base = NULL; // head of linked list

struct block_meta {
  size_t size;
  struct block_meta *next;
  int free;
};

/* find_free_block: iterates through linked list looking
  for free block of correct size

  returns: free block
*/
struct block_meta *find_free_block(struct block_meta **last, size_t size) {
  struct block_meta *current = global_base;

  while(current && !(current->free && current->size >= size)) {
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
struct block_meta *request_space(sturct block_meta last, size_t size) {
  struct block_meta *block;
  block = sbrk(0); // points to current top of heap
  void *request = sbrk(size + META_SIZE);
  assert((void*)block == request);
  if(request == (void*) - 1) {
    return NULL; // sbrk failed
  }

  if(last) { // add to linked list if not first request
    last->next = block
  }

  block->size = size;
  block->next = NULL;
  block->free = 0;
  return block;
}

/* malloc: allocates space on heap using sbrk

  size_t: number of bytes to allocate

  returns: pointer to allocated space
*/
void *malloc(size_t size) {
  struct block_meta block;
  if(size <= 0) {
    return NULL;
  }

  if(!global_base){ // if first call
    block = request_space(NULL, size);
    if(!block){ //request space failed?
      return NULL;
    }
    global_base = block;
  } else {
    struct block_meta *last = global_base;
    block = find_free_block(&last, size);
    if(!block) { // failed to find free block
      block = request_space(NULL, size);
      if(!block){
        return NULL;
      }
    } else { // found free block
      block->free = 0;
    }
  }
  return(block+1);
  // block+1 because want to point to the region after block_meta
  // and +1 increments by one sizeof(struct(block_meta))
}

struct block_meta *get_block_ptr(void *ptr) {
  return (struct block_meta*)ptr-1;
}

// free: frees block at given pointer
void free(void*ptr) {
  if(!ptr) {
    return;
  }
  struct block_meta* block_ptr = get_block_ptr(ptr);
  assert(block_ptr->free == 0);
  block_ptr->free = 1;
}
