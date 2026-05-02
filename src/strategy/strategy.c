/* =========================================================================
 * FILE: strategy.c
 * PURPOSE: Implements the O(1) Segregated List search algorithm.
 * ========================================================================= */

#include "strategy.h"
#include <stdint.h>  // SIZE_MAX

/**
 * @brief Finds a free block using Segregated Free Lists in O(1) time.
 * 
 * Maps the requested size to an exact array index. If the bin has a block, 
 * it pops it instantly. If empty, it checks the next largest bin. Only falls 
 * back to an O(N) search if it hits the massive Overflow Bin.
 * 
 * @param size The requested allocation size (must be 8-byte aligned).
 * @return block_header_t* Pointer to the suitable block, or NULL if exhausted.
 */
INTERNAL block_header_t *find_free_block(size_t size)
{
    size_t start_bin = get_bin_index(size);

    // 1. O(1) FAST PATH: Search the exact bin, and then larger exact bins.
    for (size_t i = start_bin; i < OVERFLOW_BIN; i++)
    {
        if (bins[i] != NULL)
        {
            // We found a block in an exact-size bin! 
            // Because bins hold specific sizes, the first block is guaranteed to fit.
            return bins[i]; 
        }
    }

    // 2. SLOW PATH: If no exact bins have space, we must search the Overflow Bin.
    // This bin contains all massive blocks (> 512B) in a linked list.
    block_header_t *curr = bins[OVERFLOW_BIN];
    block_header_t *best_fit = NULL;
    size_t best_size = SIZE_MAX;

    while (curr != NULL)
    {
        if (curr->size >= size)
        {
            if (curr->size == size)
            {
                return curr; // Exact match in overflow
            }
            if (curr->size < best_size)
            {
                best_fit = curr;
                best_size = curr->size;
            }
        }
        curr = curr->free_next; // Notice we traverse free_next, NOT physical next!
    }

    return best_fit;
}

// /**
//  * @brief Finds the best-fitting free block for a given size.
//  * 
//  * Scans the entire heap to find the smallest free block that is greater 
//  * than or equal to the requested size. Halts immediately on an exact match.
//  * 
//  * @param size The requested allocation size (must be 8-byte aligned).
//  * @return block_header_t* Pointer to the best-fit block, or NULL if no 
//  *         suitable block exists.
//  */
// Collective Best-fit strategy: finds the free block with the smallest size that still fits
// INTERNAL block_header_t *find_free_block(size_t size)
// {
//     block_header_t *curr = heap_start;
//     block_header_t *best_fit = NULL;
//     size_t best_size = SIZE_MAX;
    
//     while(curr != NULL)
//     {
//         if(curr->is_free && curr->size >= size)
//         {
//             if(curr->size == size)
//             {
//                 return curr;
//             }

//             if(curr->size < best_size)
//             {
//                 best_fit = curr;
//                 best_size = curr->size;
//             }
//         }
//         curr = curr->next;
//     }
//     return best_fit;
// }