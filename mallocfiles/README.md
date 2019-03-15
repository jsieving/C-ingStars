## Memory Allocator Implementation and Tests

####  by Jane Sieving and Camille Xue

my_malloc is our (very, very simple) implementation of malloc. It's a work in progress, and it's based on [this tutorial](https://danluu.com/malloc-tutorial/) by Dan Luu.

`make` or `make test1` will compile my_malloc and test1 which uses my_malloc.

`make malloc_test` will compile my_malloc and malloc_test which evaluates my_malloc.

`make bigint` will compile my_malloc and bigint which uses my_malloc.

`make my_malloc` will compile my_malloc to a shared library
It's not recommended to use this in place of malloc for your whole environment, as it doesn't reliably work.

`make clean` deletes files created by make.
