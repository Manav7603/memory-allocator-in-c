/* =========================================================================
 * FILE: malloc.h
 * PURPOSE: The public API for the custom memory allocator.
 * Changes: 'block_header_t' is completely removed from this file.
 * The user has no idea how your memory is structured. This prevents them 
 * from accidentally (or maliciously) altering block metadata.
 * ========================================================================= */

#ifndef MALLOC_H
#define MALLOC_H

#include <stddef.h> // for size_t

// PUBLIC API's
// Only these 4 functions are exported to the outside world.

/**
 * @brief Allocates a block of memory of the specified size.
 * 
 * Memory is guaranteed to be 8-byte aligned. If the size is 0, 
 * or if memory cannot be allocated, NULL is returned.
 * 
 * @param size The number of bytes to allocate.
 * @return void* Pointer to the allocated memory, or NULL on failure.
 */

void *my_malloc(size_t size);

/**
 * @brief Deallocates a previously allocated memory block.
 * 
 * Automatically triggers bidirectional coalescing to merge adjacent 
 * free blocks and reduce external fragmentation. Does nothing if ptr is NULL.
 * 
 * @param ptr Pointer to the memory block to free.
 */
void my_free (void *ptr);

/**
 * @brief Allocates memory for an array and initializes it to zero.
 * 
 * Includes integer overflow protection.
 * 
 * @param nelem Number of elements in the array.
 * @param size Size of each element in bytes.
 * @return void* Pointer to the zeroed memory, or NULL on failure/overflow.
 */
void *my_calloc(size_t nelem, size_t size);

/**
 * @brief Resizes a previously allocated memory block.
 * 
 * Will resize in-place if adjacent free space allows, or allocate a 
 * new block and migrate the data.
 * 
 * @param ptr Pointer to the original memory block.
 * @param size The new requested size.
 * @return void* Pointer to the resized memory, or NULL on failure.
 */
void *my_realloc(void *ptr, size_t size);

#endif