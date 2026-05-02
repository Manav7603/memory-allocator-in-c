/* =========================================================================
 * FILE: block.c
 * PURPOSE: Implements core block operations: alignment, splitting, coalescing.
 * ========================================================================= */

#include "block.h"


// Initialize the global array of bins to NULL
block_header_t *bins[NUM_BINS + 1] = {NULL};

INTERNAL size_t align8(size_t size)
{
    return (size + (ALIGNMENT - 1)) & ~((size_t)(ALIGNMENT - 1));
}

INTERNAL block_header_t *get_last_block(void)
{
    block_header_t *curr = heap_start;
    if(curr == NULL) return NULL;
    while(curr->next != NULL) curr = curr->next;
    return curr;
}

INTERNAL size_t get_bin_index(size_t size)
{
    if (size == 0) return 0;
    if (size >= MAX_BIN_SIZE) return OVERFLOW_BIN;
    return (size / ALIGNMENT) - 1; 
}

INTERNAL void insert_free_block(block_header_t *block)
{
    if (block == NULL) return;
    
    size_t index = get_bin_index(block->size);
    block->free_next = bins[index];
    block->free_prev = NULL;

    if (bins[index] != NULL)
    {
        bins[index]->free_prev = block;
    }
    bins[index] = block;
}

INTERNAL void remove_free_block(block_header_t *block)
{
    if (block == NULL) return;

    size_t index = get_bin_index(block->size);

    if (block->free_prev != NULL)
    {
        block->free_prev->free_next = block->free_next;
    }
    else
    {
        bins[index] = block->free_next;
    }

    if (block->free_next != NULL)
    {
        block->free_next->free_prev = block->free_prev;
    }

    block->free_next = NULL;
    block->free_prev = NULL;
}

INTERNAL void split_block(block_header_t *block, size_t size)
{
    if(block == NULL) return;

    if(block->size < size + sizeof(block_header_t) + MIN_SPLIT_SIZE) return;

    // Note: 'block' has already been removed from bins by malloc.c before entering this function.

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

    // Insert the leftover free chunk into its new, smaller bin
    insert_free_block(new_block);
}

INTERNAL void coalesce(block_header_t *block)
{
    if(block == NULL) return;

    // NOT calling remove_free_block(block) here.
    // 'block' was just freed by the user and is not in any bin yet.

    // Merge with physical NEXT block
    if(block->next != NULL && block->next->is_free)
    {
        // The Memory Contiguity Check
        // Ensure the end of 'block' exactly touches the start of 'next'
        if ((char *)block + sizeof(block_header_t) + block->size == (char *)block->next)
        {
            block_header_t *next = block->next;
            remove_free_block(next); // Safe: next is currently in a bin

            block->size += (sizeof(block_header_t) + next->size);
            block->next = next->next;

            if(block->next != NULL)
            {
                block->next->prev = block;
            }
        }
    }

    // Merge with physical PREVIOUS block
    if(block->prev != NULL && block->prev->is_free)
    {
        // BUG 2 FIXED: The Memory Contiguity Check
        // Ensure the end of 'prev' exactly touches the start of 'block'
        if ((char *)block->prev + sizeof(block_header_t) + block->prev->size == (char *)block)
        {
            block_header_t *prev = block->prev;
            remove_free_block(prev); // Safe: prev is currently in a bin

            prev->size += sizeof(block_header_t) + block->size;
            prev->next = block->next;

            if(prev->next != NULL)
            {
                prev->next->prev = prev;
            }

            block = prev;
        }
    }

    // Finally, insert the fully merged block into the correct bin
    insert_free_block(block);
}

// /**
//  * @brief Rounds a requested size up to the nearest multiple of 8.
//  * 
//  * Ensures all memory addresses returned to the user are properly aligned 
//  * for hardware efficiency and vector instruction compatibility.
//  * 
//  * @param size The raw requested size.
//  * @return size_t The 8-byte aligned size.
//  */
// // Round a size up to the nearest multiple of 8
// INTERNAL size_t align8(size_t size)
// {
//     return (size + (ALIGNMENT - 1)) & ~((size_t)(ALIGNMENT - 1));
// }


// /**
//  * @brief Traverses the heap to find the very last block.
//  * 
//  * @return block_header_t* Pointer to the last block, or NULL if heap is empty.
//  */
// // Return the last block in the heap linked list
// INTERNAL block_header_t *get_last_block(void)
// {
//     block_header_t *curr = heap_start;

//     if(curr == NULL)
//     {
//         return NULL;
//     }

//     while(curr->next != NULL)
//     {
//         curr = curr->next;
//     }

//     return curr;
// }


// /**
//  * @brief Splits a large free block to carve out a specific size.
//  * 
//  * Includes Splinter Prevention: Refuses to split if the remaining space 
//  * is too small to hold a new block header and minimum payload.
//  * 
//  * @param block Pointer to the free block being split.
//  * @param size The payload size being carved out (aligned).
//  */
// // Splitting will happen only if the requested size is large enough to hold another header plus MIN_SPLIT_SIZE Bytes;
// INTERNAL void split_block(block_header_t *block, size_t size)
// {
//     if(block == NULL)
//     {
//          return;
//     }

//     // Splinter prevention
//     if(block->size < size + sizeof(block_header_t) + MIN_SPLIT_SIZE)
//     {
//         return;
//     }

//     block_header_t *new_block = (block_header_t *)((char *)(block + 1) + size); 

//     new_block->size = block->size - size - sizeof(block_header_t);
//     new_block->is_free = 1;
//     new_block->next = block->next;
//     new_block->prev = block;

//     if(new_block->next != NULL)
//     {
//         new_block->next->prev = new_block;
//     }

//     block->next = new_block;
//     block->size = size;
// }


// /**
//  * @brief Merges a freed block with adjacent free blocks.
//  * 
//  * Checks both forward and backward in the heap. If neighbors are also 
//  * marked as free, their metadata is combined into a single, massive block 
//  * to combat external fragmentation.
//  * 
//  * @param block Pointer to the block that was just freed.
//  */
// // Coalesce: Merging a free block with nearby free block 
// INTERNAL void coalesce(block_header_t * block)
// {
//     if(block == NULL)
//     {
//         return;
//     }

//     // Merge with next free block if possible
//     if(block->next != NULL && block->next->is_free)
//     {
//         block_header_t *next = block->next;
//         block->size += (sizeof(block_header_t) + next->size);
//         block->next = next->next;

//         if(block->next != NULL)
//         {
//             block->next->prev = block;
//         }
//     }

//     // Merge with previous free block if possible
//     if(block->prev != NULL && block->prev->is_free)
//     {
//         block_header_t *prev = block->prev;
//         prev->size += sizeof(block_header_t) + block->size;
//         prev->next = block->next;

//         if(prev->next != NULL)
//         {
//             prev->next->prev = prev;
//         }

//         block = prev;

        
//         // Note: If block B is freed between free blocks A and C, 
//         // it merges into A, and then this block checks forward to merge with C immediately.
//         if(block->next != NULL && block->next->is_free)
//         {
//             block_header_t *next = block->next;
//             block->size += (sizeof(block_header_t) + next->size);
//             block->next = next->next;

//             if(block->next != NULL)
//             {
//                 block->next->prev = block;
//             }
//         }
//     }
// }