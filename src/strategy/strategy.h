/* =========================================================================
 * FILE: strategy.h
 * PURPOSE: Defines the algorithms used to find suitable free blocks.
 * ========================================================================= */

#ifndef STRATEGY_H
#define STRATEGY_H

#include "../block/block.h"
#include <stddef.h>


/**
 * @brief Searches the heap for an available free block.
 * 
 * Pluggable search strategy (currently implementing Best-Fit). Scans the 
 * free list to find the optimal block that minimizes internal fragmentation.
 * 
 * @param size The strictly 8-byte aligned payload size requested.
 * @return block_header_t* Pointer to the suitable free block, or NULL if 
 *         the heap must be expanded.
 */
INTERNAL block_header_t *find_free_block(size_t size);

#endif