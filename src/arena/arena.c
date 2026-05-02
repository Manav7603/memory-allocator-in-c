/* =========================================================================
 * FILE: arena.c
 * PURPOSE: Implements memory requests to the Linux Kernel using Arenas.
 * ENHANCEMENT: We now request memory in massive chunks (Arenas) and 
 * instantly split them. This destroys the mmap overhead bottleneck!
 * ========================================================================= */

#include "arena.h"
#include <sys/mman.h>  // mmap

// We define an Arena size of 4 Megabytes. 
#define ARENA_SIZE (4 * 1024 * 1024)

// Standard OS page size for alignment (4 KB)
#define PAGE_SIZE 4096

INTERNAL block_header_t *request_space(block_header_t *last, size_t size)
{
    // 1. How much memory do we actually need at a minimum?
    size_t map_size = sizeof(block_header_t) + size;

    // 2. The Arena Optimization
    // If the requirement is smaller than our Arena, we round up to 4MB!
    if (map_size < ARENA_SIZE) 
    {
        map_size = ARENA_SIZE;
    }
    else
    {
        // If they ask for a massive chunk (> 4MB), we align the request 
        // to the nearest strict OS page boundary using bitwise math.
        map_size = (map_size + (PAGE_SIZE - 1)) & ~((size_t)(PAGE_SIZE - 1));
    }

    // 3. Ask the OS for the chunk ONCE
    void * block_mem = mmap(NULL, map_size,
                           PROT_READ | PROT_WRITE,
                           MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if(block_mem == MAP_FAILED) 
    {
        return NULL;
    }

    // 4. Setup the massive block
    block_header_t * block = (block_header_t *)block_mem;
    
    // The usable size is the massive mapped chunk minus our header
    block->size = map_size - sizeof(block_header_t);
    block->is_free = 0; // Mark as allocated for now
    block->next = NULL;
    block->prev = last;

    // 5. Connect it to the rest of the heap
    if(last != NULL)
    {
        last->next = block;
    }

    // Gothroug: 
    // We gave 'block' 4MB. The user only wanted 'size' (e.g., 32 bytes).
    // We instantly call split_block. This shrinks 'block' down to 32 bytes,
    // and creates a new 3.99MB block right next to it marked as FREE!
    // The existing 'find_free_block' logic will naturally use that free space next time.
    if (block->size > size) 
    {
        split_block(block, size);
    }

    return block;
}