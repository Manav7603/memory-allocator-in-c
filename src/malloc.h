// A header file contains the interface
// Usage: Security and seperation of implementation
// Reusability

#ifndef MALLOC_H
#define MALLOC_H
// Importing multiple headers together throws error, hence we use this to ensure each file is included only once.

#include <stddef.h> // for size_t
#include <unistd.h> // for size_t


// BLOCK HEADER STRUCTURE 
typedef struct block_header
{
    size_t size;                       // Dynamic size of user data
    int is_free;                       // 1 = free, 0 = allocated 
    struct block_header *next;         // next block in the list
    struct block_header *prev;         // previous block 
} block_header_t;

// GLOBAL HEAP START
extern block_header_t *heap_start;
// The variable heap_start will be defined in other file (malloc.c) but it will be accessible everywhere

// PUBLIC API's

void *my_malloc(size_t size);
void my_free (void *ptr);
void *my_calloc(size_t nelem, size_t size);
void *my_realloc(void *ptr, size_t size);

#endif