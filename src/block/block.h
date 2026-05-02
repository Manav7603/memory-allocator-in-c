/* =========================================================================
 * FILE: block.h
 * PURPOSE: Internal definitions for memory blocks and the global heap.
 * ========================================================================= */

#ifndef BLOCK_H
#define BLOCK_H

#include <stddef.h>
#include "../internal_config.h"

#define ALIGNMENT 8
#define MIN_SPLIT_SIZE 8

// BLOCK HEADER STRUCTURE 
// Fully defined here in the private source folder.

/**
 * @struct block_header
 * @brief The metadata header attached to every allocated and free block.
 * 
 * This structure sits strictly behind the pointer returned to the user.
 * It manages the doubly-linked list of the heap state.
 */
typedef struct block_header
{
    size_t size;                       // Dynamic size of user data
    int is_free;                       // 1 = free, 0 = allocated 
    struct block_header *next;         // next block in the list
    struct block_header *prev;         // previous block 
} block_header_t;

// GLOBAL HEAP START
extern block_header_t *heap_start;

// Internal API secured with the INTERNAL macro
INTERNAL size_t align8(size_t size);
INTERNAL block_header_t *get_last_block(void);
INTERNAL void split_block(block_header_t *block, size_t size);
INTERNAL void coalesce(block_header_t * block);

#endif