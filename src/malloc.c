#include "malloc.h"
#include <stdint.h>  // SIZE_MAX
#include <string.h> // memset, memcpy

// The first block in our heap managed linked list
// NULL means no allocation till now

block_header_t *head_start = NULL;

#define ALIGNMENT 8
#define MIN_SPLIT_SIZE 8

// Round a size up to the nearest multiple of 8
// This help allocations alligned
static size_t align8(size_t size)
{
    return (size + (ALIGNMENT - 1)) & ~((size_t)(ALIGNMENT - 1));

}

// Return the last block in the heap linked list
static block_header_t *get_last_block(void)
{
    block_header_t *curr =heap_start;

    if(curr==NULL)
    {
        return NULL;
    }

    while(curr->next != NULL)
    {
        curr =curr->next;
    }

    return curr;
}


// Finding the first free block that satisfy the request i.e 1st free block whose size is greater than the requested size
// For now making it first fit strategy

static block_header_t *find_free_block (size_t size)
{
    block_header_t *curr= heap_start;
    while(curr != NULL)
    {
        if(curr->is_free && curr->size >= size)
        {
            return curr;
        }

    }
    return NULL;
}

// Asking the "OS" for more heap memory using sbrk
// Returned block contains both the header and the user region

static block_header_t *request_space(block_header_t *last,size_t size)
{
    void * block_mem=(sbrk(sizeof(block_header_t)+size));

    
}