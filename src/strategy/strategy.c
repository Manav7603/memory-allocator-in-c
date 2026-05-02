/* =========================================================================
 * FILE: strategy.c
 * PURPOSE: Implements the Best-Fit block search algorithm.
 * ========================================================================= */

#include "strategy.h"
#include <stdint.h>  // SIZE_MAX


/**
 * @brief Finds the best-fitting free block for a given size.
 * 
 * Scans the entire heap to find the smallest free block that is greater 
 * than or equal to the requested size. Halts immediately on an exact match.
 * 
 * @param size The requested allocation size (must be 8-byte aligned).
 * @return block_header_t* Pointer to the best-fit block, or NULL if no 
 *         suitable block exists.
 */
// Collective Best-fit strategy: finds the free block with the smallest size that still fits
INTERNAL block_header_t *find_free_block(size_t size)
{
    block_header_t *curr = heap_start;
    block_header_t *best_fit = NULL;
    size_t best_size = SIZE_MAX;
    
    while(curr != NULL)
    {
        if(curr->is_free && curr->size >= size)
        {
            if(curr->size == size)
            {
                return curr;
            }

            if(curr->size < best_size)
            {
                best_fit = curr;
                best_size = curr->size;
            }
        }
        curr = curr->next;
    }
    return best_fit;
}