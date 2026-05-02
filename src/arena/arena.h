/* =========================================================================
 * FILE: arena.h
 * PURPOSE: Boundary between the allocator and the Operating System.
 * ========================================================================= */

#ifndef ARENA_H
#define ARENA_H

#include "../block/block.h"
#include <stddef.h>

/**
 * @brief Requests raw memory from the Operating System.
 * 
 * Acts as the sole interface for mmap/sbrk system calls. Uses an Arena 
 * allocation strategy to request large contiguous chunks (e.g., 4MB) 
 * to minimize context-switching overhead, returning a smaller carved block.
 * 
 * @param last Pointer to the current tail of the heap linked list.
 * @param size The strictly 8-byte aligned size required for the payload.
 * @return block_header_t* Pointer to the newly allocated block, or NULL on failure.
 */
INTERNAL block_header_t *request_space(block_header_t *last, size_t size);

#endif