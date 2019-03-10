bigint: bigint.c jsmalloc.c jsmalloc.h
	gcc bigint.c jsmalloc.c jsmalloc.h -o bigint
test1: test1.c jsmalloc.c jsmalloc.h
	gcc test1.c jsmalloc.c jsmalloc.h -o test1
jsmalloc: jsmalloc.c jsmalloc.h
	gcc -O0 -g -W -Wall -Wextra -shared -fPIC jsmalloc.c jsmalloc.h -o jsmalloc.so
clean:
	rm -f *.o *.so bigint test1
	ls
