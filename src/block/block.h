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

/* 
 * SEGREGATED BINS CONFIGURATION
 * Bin 0: 8 bytes, Bin 1: 16 bytes ... Bin 63: 512 bytes
 * Bin 64 (Overflow): > 512 bytes
 */
#define NUM_BINS 64
#define MAX_BIN_SIZE (NUM_BINS * ALIGNMENT)
#define OVERFLOW_BIN NUM_BINS

// BLOCK HEADER STRUCTURE 
// Fully defined here in the private source folder.

/**
 * @struct block_header
 * @brief The metadata header attached to every allocated and free block.
 * 
 * This structure sits strictly behind the pointer returned to the user.
 * It manages the doubly-linked list of the heap state.
 * * Uses Dual-Tracking: next/prev track physical neighbors to allow coalescing.
 * free_next/free_prev track Segregated List neighbors for O(1) lookups.
 */
typedef struct block_header
{
    size_t size;                       /**< Payload size in bytes */
    int is_free;                       /**< 1 if free, 0 if allocated */
    
    struct block_header *next;         /**< Physical next block in memory */
    struct block_header *prev;         /**< Physical previous block in memory */
    
    struct block_header *free_next;    /**< Next free block in the SAME bin */
    struct block_header *free_prev;    /**< Previous free block in the SAME bin */
} block_header_t;

// GLOBAL HEAP STATE
extern block_header_t *heap_start;
extern block_header_t *bins[NUM_BINS + 1];

// Internal API secured with the INTERNAL macro
INTERNAL size_t align8(size_t size);
INTERNAL block_header_t *get_last_block(void);
INTERNAL void split_block(block_header_t *block, size_t size);
INTERNAL void coalesce(block_header_t * block);
// Bin Management API
INTERNAL size_t get_bin_index(size_t size);
INTERNAL void insert_free_block(block_header_t *block);
INTERNAL void remove_free_block(block_header_t *block);
#endif