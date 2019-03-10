jsmalloc: jsmalloc.c jsmalloc.h
	gcc -O0 -g -W -Wall -Wextra -shared -fPIC jsmalloc.c jsmalloc.h -o jsmalloc.so
bigint: bigint.c
	gcc bigint.c -o bigint
clean:
	rm -f *.o *.so
