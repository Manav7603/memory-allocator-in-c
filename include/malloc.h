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

void *my_malloc(size_t size);
void my_free (void *ptr);
void *my_calloc(size_t nelem, size_t size);
void *my_realloc(void *ptr, size_t size);

#endif