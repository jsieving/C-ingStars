#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

/* simple_malloc: allocates space on heap using sbrk

  size_t: number of bytes to allocate

  returns: p pointer to block of memory of given size
*/
void *simple_malloc(size_t size) {
  void *p = sbrk(0); // returns pointer to top of heap

  // increments heap size and returns pointer to previous
  // top of the heap
  void *request = sbrk(size);

  if(request == (void*) -1) {
    return NULL;
  } else {
    assert(p == request);
    return p;
  }
}
