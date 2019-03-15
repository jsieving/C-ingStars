test1: test1.c my_malloc.c my_malloc.h
	gcc -Wall test1.c my_malloc.c my_malloc.h -o test1
bigint: bigint.c my_malloc.c my_malloc.h
	gcc -Wall bigint.c my_malloc.c my_malloc.h -o bigint
malloc_test: malloc_test.c my_malloc.c my_malloc.h
	gcc -Wall malloc_test.c my_malloc.c my_malloc.h -o malloc_test
my_malloc: my_malloc.c my_malloc.h
	gcc -O0 -g -W -Wall -Wextra -shared -fPIC my_malloc.c my_malloc.h -o my_malloc.so
clean:
	rm -f *.o *.so bigint test1 malloc_test
	ls
