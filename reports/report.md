# Implementation of Malloc with C
### Camille Xue and Jane Sieving

## Project Description

For our project we made our own implementation of malloc, free, and realloc. Our lower bound was to have a working basic implementation of malloc. For our stretch goal we would improve our implementation and/or implement more of the features of malloc and the other memory functions. For example, we could try optimizing with different fit algorithms and coalescing.

We were about to incorporate what we learned splitting, coalescing, and alignment, accomplishing that aspect of our stretch goal. However, we didn't focus as much on making improvements in terms of optimizing performance.

## Learning Goals

*Camille:* I want to have a strong understanding of C, memory, and pointers. I want to have a better understanding of how C structs are used for implementing different functions.
* Reflection: I definitely accomplished my learning goals with this project . I was able to learn a lot about dynamic memory at a conceptual level from the research that we did and our implementation itself. The block data structure used in our implementation was a key feature, which helped my understanding of structs and how they are used.

*Jane:* I want to gain a better understanding of how C handles memory under the hood and what is required to efficiently and effectively handle memory. I’m also just interested in having to write my own version of something that works at a pretty low level.
* Reflection:


## Quick Malloc Review
* `malloc` allocates a chunk of space with the given size on the heap, either by finding a free chunk of space or requesting more space using `sbrk`. It returns a pointer to the newly allocated block.
* `realloc` changes the size of an allocated chunk of memory to a new given size. It takes a pointer to the old chunk and a size and returns a pointer to a block of the requested size containing the original data (or as much fits in the new size.)
* `calloc` is similar to malloc, but it takes a number of elements and the size of each element and uses that to calculate the needed space.
* `free` deallocates a chunk so that the space is free for future allocation.


## Our Implementation
### 1. Basics: Linked list of blocks
We followed the tutorial from Resource 1 to create our first pass at our malloc implementation. This implementation handles allocated heap space as a linked list of memory blocks, each tagged by a block metadata structure containing the size of that block, pointers to the next and previous blocks, and the block's status as free or not.

```c
typedef struct block_meta {
  size_t size;
  struct block_meta *prev;
  struct block_meta *next;
  int free;
} Block;
```
Generally, we will refer to the block metadata structure and the block of memory itself collectively as a block. Data can be stored in the block of memory, and the size in the metadata refers to the size of this available memory, not including space for the metadata. Given a pointer to some data dynamically allocated by malloc, the location of the corresponding metadata can be found `META_SIZE` bytes before the pointer, where `META_SIZE` is the size of the metadata structure.

To allocate a block of information, `malloc` traverses the linked list of memory, searching for a free block of adequate size. If none is found, it uses `request_space` to get a new block on top of the heap (which calls `sbrk` to increase the size of the heap).

To free a block, `free` takes the pointer to some data and gets its metadata by subtracting `META_SIZE` from the address. Basically, all that is needed to free the space is to set the `free` attribute to 1.

In order to allocate and free space effectively and in a way that makes the best use of heap space, other checks are added and blocks are sometimes split up and joined. `realloc` behaves differently depending on whether there is adjacent space available, whether the block is on top of the stack, and whether the block is being shrunk or expanded. These complexities of our implementation are explained in the next section and in the section "Functions In-Depth".

### 2. Splitting, Coalescing & Alignment

Allocating and freeing many small blocks could lead to long runs of blocks of adjacent free space which are each too small to be utilized when a larger allocation request is executed. This means that calls to `malloc` would traverse past all of them while searching for space, and `request_space` may be called excessively because a large enough block was not found.

To fix this, we used coalescing to combine adjacent free blocks. This means that whenever a block is freed, it will check whether its neighbors are free and merge into a single block with all adjecent free space. If the next block is free, its space (including metadata space) is absorbed into the current block, and the current block's `next` attribute is set to the `next` block of the absorbed block. If that next block is not `NULL`, its `prev` attribute must also be set to point back to the current block.
```c
// If the next block is free, absorb its space
if (block_ptr->next && block_ptr->next->free == 1) {
    block_ptr->size += META_SIZE + block_ptr->next->size;
    block_ptr->next = block_ptr->next->next;
    if (block_ptr->next) {block_ptr->next->prev = block_ptr->prev;}
}
```
If the previous block is [also] free, it absorbs the space and inherits the `next` attribute from the current block. If the current block had a non-null next block, that must have its `prev` attribute changed to point back to the previous block. At this point the current block is being skipped over and its pointer will be lost after the function exits, so it does nothing when, a couple lines later, its `free` attribute is set to `1`. However, the previous block is known to be free and now owns all of its space, so we don't need a pointer to it anymore.
```c
// If the previous block is free, grow it and tell it to jump over this one
if (block_ptr->prev && block_ptr->prev->free == 1) {
    block_ptr->prev->size += META_SIZE + block_ptr->size;
    block_ptr->prev->next = block_ptr->next;
    if (block_ptr->next) {block_ptr->next->prev = block_ptr->prev;}
}
```

Another way that allocating memory could waste space is by returning pointers to unnecessarily large free blocks and then marking them as not free. To avoid this, whenever `malloc` or `realloc` use an existing free block to allocate space, they call the function `handle_extra_space`. This function checks if the given block is larger than necessary (by so much that a new block structure could be created). If it is, it creates a new block structure with any remaining space, fixes the `next` and `prev` pointers appropriately, and calls `free` on the new block. `free` is called so the new split-off block will be coalesced with any neighboring free space.
```c
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
```

Finally, when a CPU is reading memory, it is more efficient if data is aligned to the word size of the CPU so that the CPU always gets a complete piece of data. In order to keep data aligned to the CPU word size, dynamic memory should be allocated in blocks whose size are divisible by the word size. In the case of a 64-bit architecture, that is 8 bytes, so whenever a number of bytes is requested from `malloc`, `realloc` or `calloc`, the number is rounded up to the nearest multiple of 8.
```c
if (size & 0b111) {
    size = ((size >> 3) << 3) + 8; // Round size up to multiple of 8
}
```

### 3. Threading
The tutorial implementation that we followed had notes that certain aspects were not thread safe, so we explored threading as it relates to malloc. We didn't fully incorporate multithreading in `my_malloc`, but if we were to continue working on the project, it would be a good feature to complete. Here are some of the key things we learned in our research:
* We found that we should use the `<pthread.h>` library to incorporate threading. More specifically, we tried to use `pthread_mutex_lock()` function for mutual exclusion.
* *global variabes*: Mutual exclusion should be used whenever a global variable is being read or updated. In our case, we have one global variable that we change throughout the program: `global_base`. It is the head of the linked list of memory blocks.
* `sbrk` versus `mmap`: We learned that `sbrk` is not necessarily thread safe, but it is still used for malloc implementations. Generally `sbrk` is used for small allocations on the heap and `mmap` is used for large allocations.
* *arenas*: our implementation of malloc doesn't incorporate arenas, which are structures that contain one or more heaps. Threads are assigned to an arena and will only allocate memory within it.
* `assert` is also not thread safe as it's primarily for debugging, so a final implementation would not have the assertions active.
* *forking*: Another concept that came up during research was forking, since one thread could call `fork()` while another is calling `malloc`. This means that a thread-safe malloc would need to be able to handle forking as well.

## Key Functions In-Depth

### my_malloc
`void *my_malloc(size_t size);`

`my_malloc` checks if a global base block exists, and if not it creates one using request_space. If one does exist, it searches for a free block that is large enough and returns a pointer to that. If one is found, it sets its `free` attribute to 0 and calls `handle_extra_space` in case it is larger than necessary. If a free block of sufficient size is not found, one is created using `request_space`.

### my_free
`void my_free(void *ptr);`

`my_free` first checks if coalescing can be done with the next block, then also checks if it can be done with the previous block, then sets the current block `free` parameter to 1. This setting may not be meaningful if the current block has been absorbed into the previous free one.

### my_realloc
`void *my_realloc(void *ptr, size_t size);`

If a null pointer is given, `my_realloc` acts like `malloc`.

If the argument block's size is at least the requested size, `handle_extra_space` is called. This will split the block into 2 blocks if there is enough extra space to create a new block metadata structure. The second block will be free, and the first will be returned. If there is not enough extra space, the original block is returned.

If there is a free block following the argument block, and the block occupies enough space to accomodate the needed size, the original block absorbs the next block's space, `next` and `prev` pointers are changed to skip over and forget the next block, and `handle_extra_space` is called in case there is leftover space.

If the argument block is on the top of the heap, `sbrk` is called to try to expand its space.

If none of these conditions are met to make expansion easier, a new pointer is `malloc`ed, `memcpy` is used to copy the old block data to the larger block, and the old block is freed.

### my_calloc
`void *my_calloc(size_t nelem, size_t elsize);`

`my_calloc` multiplies the input arguments, calls `my_malloc` with that result, and uses `memset` to set the memory to 0.



## Test Code and Demo

We developed several tests for our code. We started by just testing if our `malloc` would work in the `bigint.c` homework, then created 3 tests to `malloc` blocks, `free` them, and `malloc` them again in various orders. After getting these to pass, we created `test4.c`, which uses `malloc`, `free`, `realloc` and `calloc`. The output from this test is in `test_results.txt` in the `reports` directory of this repo. It demonstrates using `malloc` to create several pointers, the different behavior of `realloc` based on available space, using `calloc`, and then freeing all the allocated pointers.


## Resources
We have found some resources and tutorials on implementing malloc, and we have followed the tutorial on the first resource as our main starting point.
1. [Dan Luu's Malloc Tutorial](https://danluu.com/malloc-tutorial/)
We followed this step-by-step tutorial as our starting point for our basic implementation before adding other features. There are also exercises at the end of the tutorial.
2. [Dynamic Memory Allocation Lecture Notes](https://www.cs.cmu.edu/~guna/15-123S11/Lectures/Lecture08.pdf)
Ananda Gunawardena's lecture notes about dynamic memory allocation.
3. [Implementing malloc](http://moss.cs.iit.edu/cs351/slides/slides-malloc.pdf)
Michael Saelee's lecture slides about implementing malloc. It has good visuals and explanations about the basics of malloc as well as splitting, fit, and coalescing. It also discusses the runtimes and tradeoffs of different implementations.
4. [Dynamic Memory Allocation: Basic Concepts](http://www.cs.cmu.edu/afs/cs/academic/class/15213-f11/www/lectures/18-allocation-basic.pdf)
Another set of lecture slides for understanding dynamic memory allocation. It explains splitting, fit, and coalescing.
5. [Overview of GNU C Malloc](https://sourceware.org/glibc/wiki/MallocInternals)
Detailed description of the GNU C malloc library. It defines key terms and explains the algorithms used for each function, so it's a really good resource to start with.
7. [Saven Patel's Thread-Safe Malloc](https://github.com/savanpatel/malloc)
We referenced Savan Patel's implementation to see how threading is incorporated.
6. [ptmalloc](http://www.malloc.de/en/)
We also refereced `ptmalloc` since it can be used with multiple threads, but it was harder to navigate and understand all of the code in comparison to Resource 5.
