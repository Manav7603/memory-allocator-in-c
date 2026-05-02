#include "strategy.h"
#include <stdint.h>  // SIZE_MAX

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