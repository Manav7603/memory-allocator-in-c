/* =========================================================================
 * FILE: malloc.c
 * PURPOSE: The main API gateway and traffic controller for the allocator.
 * ========================================================================= */

#include "../include/malloc.h"
#include "block/block.h"
#include "arena/arena.h"
#include "strategy/strategy.h"
#include <stdint.h>  // SIZE_MAX
#include <string.h>  // memset, memcpy


/**
 * @var heap_start
 * @brief Global pointer to the start of the allocator's linked list.
 * 
 * Represents the root of the entire heap state. All block searches and 
 * coalescing operations traverse this structure.
 */
// The first block in our heap managed linked list
block_header_t *heap_start = NULL;


/* =========================================================================
 * PUBLIC API IMPLEMENTATIONS
 * Detailed interface documentation is located in include/malloc.h
 * ========================================================================= */


// Default compiler visibility is kept here so the user can call these.

// The main program will call this 'my_malloc' function
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

    // 1st allocation: Create the initial block if it the list is empty

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

    // If the list if alreay there:
    // Trying to reuse the exsisting free blocks
    // O(1) Search for an existing free block
    block = find_free_block(aligned_size);
    if(block != NULL)
    {
        // CRITICAL UPDATE: We found a block. We MUST remove it from the free bins
        // immediately so no other allocation can claim it!
        remove_free_block(block);

        // Split the block if it is too large. 
        // (split_block will automatically insert the leftover piece back into the bins).

        split_block(block, aligned_size);
        block->is_free = 0;
        return (void *)(block + 1);
    }

    // Still not suitable memory block found, so requestiong fresh memory from OS;  No free blocks available, ask the OS for a new 4MB Arena

    block_header_t *last = get_last_block();
    block = request_space(last, aligned_size);

    if(block == NULL)
    {
        return NULL;
    }

    return (void *)(block + 1);
}


// As no garbage collector, manually the code will need to free the space which was previously allocated by 'my_alloc'

__attribute__((visibility("default"))) 
void my_free(void *ptr)
{
    block_header_t * block;

    if(ptr == NULL)
    {
        return;
    }

    // This will be user pointer
    // So first step will be manually steping back so we point it to block header

    block = ((block_header_t *)ptr) - 1;
    // Everwhere we are using pointer arithmetic so the C compiler knows that to come back a whole unit of 'block_header_t'

    block->is_free = 1;

    // Now let's try to merge them
    // Merge physical neighbors, which automatically inserts the 
    // resulting massive block into the correct O(1) bin!
    coalesce(block);
}


// Custom calloc which will set default value 0 in the memory before giving it to code

__attribute__((visibility("default"))) 
void *my_calloc(size_t nelem, size_t elem_size)
{
    size_t total;
    void *ptr;

    // To Prevent integer overflow in multiplication
    // This overflow check is needed
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

// Multi purpose "Realloc"
__attribute__((visibility("default"))) 
void *my_realloc(void *ptr, size_t size)
{
    block_header_t * block;
    void * new_ptr;
    size_t aligned_size;
    size_t copy_size;

    // realloc(NULL, Size) behaves like malloc(size)
    if(ptr == NULL)
    {
        return my_malloc(size);
    }
    // realloc(ptr, 0) behaves like free(ptr)
    if(size == 0)
    {
        my_free(ptr);
        return NULL;
    }

    aligned_size = align8(size);
    block = ((block_header_t *)ptr) - 1;

    // If the current block is large enough, Reuse it
    // Also split it if any leftover space is there
    // In-place split
    if(block->size >= aligned_size)
    {
        split_block(block, aligned_size);
        return ptr;
    }

    // If not allocate a new block, copy data as it is, free old block

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