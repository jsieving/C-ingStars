test1: test1.c my_malloc.c my_malloc.h
	gcc -g -Wall test1.c my_malloc.c my_malloc.h -o test1
test2: test2.c my_malloc.c my_malloc.h
	gcc -g -Wall test2.c my_malloc.c my_malloc.h -o test2
test3: test3.c my_malloc.c my_malloc.h
	gcc -g -Wall test3.c my_malloc.c my_malloc.h -o test3
test4: test4.c my_malloc.c my_malloc.h
	gcc -g -Wall test4.c my_malloc.c my_malloc.h -o test4
bigint: bigint.c my_malloc.c my_malloc.h
	gcc -g -Wall bigint.c my_malloc.c my_malloc.h -o bigint
malloc_test: malloc_test.c my_malloc.c my_malloc.h
	gcc -g -Wall malloc_test.c my_malloc.c my_malloc.h -o malloc_test
my_malloc: my_malloc.c my_malloc.h
	gcc -O0 -g -W -Wall -Wextra -shared -fPIC my_malloc.c my_malloc.h -o my_malloc.so
clean:
	rm -f *.o *.so bigint test1 test2 test3 malloc_test
	ls
