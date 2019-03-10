## Memory Allocator Implementation and Tests

####  by Jane Sieving

jsmalloc is my (very, very simple) implementation of malloc. It's a work in progress, and it's based on [this tutorial](https://danluu.com/malloc-tutorial/) by Dan Luu. It's also only one side of this project, and desperately wants to be combined with Camille's work to improve the implementation.

`make` or `make test1` will compile jsmalloc and test1 which uses jsmalloc.

`make bigint` will compile jsmalloc and bigint which uses jsmalloc.

`make jsmalloc` will compile jsmalloc to a shared library
It's not recommended to use this in place of malloc for your whole environment, as it doesn't reliably work.

`make clean` deletes files created by make.
