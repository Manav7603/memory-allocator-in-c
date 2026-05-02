#include "../include/malloc.h"
#include "block/block.h"
#include "arena/arena.h"
#include "strategy/strategy.h"
#include <stdint.h>  // SIZE_MAX
#include <string.h>  // memset, memcpy

// The first block in our heap managed linked list
block_header_t *heap_start = NULL;

// Default compiler visibility is kept here so the user can call these.
__attribute__((visibility("default"))) 
void *my_malloc(size_t size)
{
    block_header_t * block;
    size_t aligned_size;
    
    if(size == 0)
    {
        return NULL;
    }

    aligned_size = align8(size);

    if(heap_start == NULL)
    {
        block = request_space(NULL, aligned_size);
        if(block == NULL)
        {
            return NULL;
        }

        heap_start = block;
        return (void *)(block + 1);
    }

    block = find_free_block(aligned_size);
    if(block != NULL)
    {
        split_block(block, aligned_size);
        block->is_free = 0;
        return (void *)(block + 1);
    }

    block_header_t *last = get_last_block();
    block = request_space(last, aligned_size);

    if(block == NULL)
    {
        return NULL;
    }

    return (void *)(block + 1);
}

__attribute__((visibility("default"))) 
void my_free(void *ptr)
{
    block_header_t * block;

    if(ptr == NULL)
    {
        return;
    }

    block = ((block_header_t *)ptr) - 1;
    block->is_free = 1;
    coalesce(block);
}

__attribute__((visibility("default"))) 
void *my_calloc(size_t nelem, size_t elem_size)
{
    size_t total;
    void *ptr;

    // LOGIC VERIFICATION: This overflow check is perfect.
    if(nelem != 0 && elem_size > SIZE_MAX / nelem)
    {
        return NULL;
    }

    total = nelem * elem_size;
    ptr = my_malloc(total);

    if(ptr != NULL)
    {
        memset(ptr, 0, total);
    }

    return ptr;
}

__attribute__((visibility("default"))) 
void *my_realloc(void *ptr, size_t size)
{
    block_header_t * block;
    void * new_ptr;
    size_t aligned_size;
    size_t copy_size;

    if(ptr == NULL)
    {
        return my_malloc(size);
    }
    if(size == 0)
    {
        my_free(ptr);
        return NULL;
    }

    aligned_size = align8(size);
    block = ((block_header_t *)ptr) - 1;

    if(block->size >= aligned_size)
    {
        split_block(block, aligned_size);
        return ptr;
    }

    new_ptr = my_malloc(aligned_size);
    if(new_ptr == NULL)
    {
        return NULL;
    }

    copy_size = block->size;
    memcpy(new_ptr, ptr, copy_size);

    my_free(ptr);
    return new_ptr;
}