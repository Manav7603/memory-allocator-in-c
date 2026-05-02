#include "block.h"

// Round a size up to the nearest multiple of 8
INTERNAL size_t align8(size_t size)
{
    return (size + (ALIGNMENT - 1)) & ~((size_t)(ALIGNMENT - 1));
}

// Return the last block in the heap linked list
INTERNAL block_header_t *get_last_block(void)
{
    block_header_t *curr = heap_start;

    if(curr == NULL)
    {
        return NULL;
    }

    while(curr->next != NULL)
    {
        curr = curr->next;
    }

    return curr;
}

// Splitting will happen only if the requested size is large enough to hold another header plus MIN_SPLIT_SIZE Bytes;
INTERNAL void split_block(block_header_t *block, size_t size)
{
    if(block == NULL)
    {
         return;
    }

    // Splinter prevention
    if(block->size < size + sizeof(block_header_t) + MIN_SPLIT_SIZE)
    {
        return;
    }

    block_header_t *new_block = (block_header_t *)((char *)(block + 1) + size); 

    new_block->size = block->size - size - sizeof(block_header_t);
    new_block->is_free = 1;
    new_block->next = block->next;
    new_block->prev = block;

    if(new_block->next != NULL)
    {
        new_block->next->prev = new_block;
    }

    block->next = new_block;
    block->size = size;
}

// Coalesce: Merging a free block with nearby free block 
INTERNAL void coalesce(block_header_t * block)
{
    if(block == NULL)
    {
        return;
    }

    // Merge with next free block if possible
    if(block->next != NULL && block->next->is_free)
    {
        block_header_t *next = block->next;
        block->size += (sizeof(block_header_t) + next->size);
        block->next = next->next;

        if(block->next != NULL)
        {
            block->next->prev = block;
        }
    }

    // Merge with previous free block if possible
    if(block->prev != NULL && block->prev->is_free)
    {
        block_header_t *prev = block->prev;
        prev->size += sizeof(block_header_t) + block->size;
        prev->next = block->next;

        if(prev->next != NULL)
        {
            prev->next->prev = prev;
        }

        block = prev;

        
        // Note: If block B is freed between free blocks A and C, 
        // it merges into A, and then this block checks forward to merge with C immediately.
        if(block->next != NULL && block->next->is_free)
        {
            block_header_t *next = block->next;
            block->size += (sizeof(block_header_t) + next->size);
            block->next = next->next;

            if(block->next != NULL)
            {
                block->next->prev = block;
            }
        }
    }
}