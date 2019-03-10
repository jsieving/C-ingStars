malloc.o: malloc.h malloc.c
	gcc -g -c malloc.c
malloc_test.o: malloc.h malloc_test.c
	gcc -c malloc_test.c
malloc_test: malloc_test.o malloc.o
	gcc -g -Wall -o malloc_test malloc.o malloc_test.o
