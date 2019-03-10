# Implementation of Malloc with C
## Camille Xue and Jane Sieving

## Project Idea

For our project we will be making our own implementation of malloc, free, and realloc. Our lower bound is to have a working basic implementation of malloc. For our stretch goal we would improve our implementation and/or implement more of the features of malloc and the other memory functions. For example, we could try optimizing with different fit algorithms and coalescing.

## Learning Goals

*Camille:* I want to have a strong understanding of C, memory, and pointers. I want to have a better understanding of how C structs are used for implementing different functions.

*Jane:* I want to gain a better understanding of how C handles memory under the hood and what is required to efficiently and effectively handle memory. I’m also just interested in having to write my own version of something that works at a pretty low level.

## Resources

We have found some resources and tutorials on implementing malloc, and we have followed the tutorial on the first resource as our main starting point.
1. https://danluu.com/malloc-tutorial/
2. http://csapp.cs.cmu.edu/3e/malloclab.pdf
3. https://www.cs.cmu.edu/~guna/15-123S11/Lectures/Lecture08.pdf
4. http://moss.cs.iit.edu/cs351/slides/slides-malloc.pdf

## What we’ve done
* We followed the tutorial from Resource 1 to create our first pass at our malloc implementation.
* We tried testing our implementation based on the steps from the tutorial, but hit a roadblock that made it an ineffective use of our time. Instead we:
* Created some of our own malloc tests to debug our implementation, show its abilities and limitations, and improve it.

## What we’re working on
* We are still creating more tests to analyze and strength-test our implementation.
* We are learning how splitting, fitting and coalescing work so we can use them to improve
our implementation.
* We are researching how to make our implementation thread-safe.

## What we're doing next
* Incorporate splitting/fit/coalescing into our implementation.
* Make our implementation thread-safe.
* Use benchmark tests from outside sources to test the performance of our malloc.
* Create a package with our malloc, test/demo files, documentation and a makefile.
