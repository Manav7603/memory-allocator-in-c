#include "arena.h"
#include <sys/mman.h>  // mmap

// Asking the "OS" for more heap memory using mmap
INTERNAL block_header_t *request_space(block_header_t *last, size_t size)
{
    void * block_mem = mmap(NULL, sizeof(block_header_t) + size,
                           PROT_READ | PROT_WRITE,
                           MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if(block_mem == MAP_FAILED) 
    {
        return NULL;
    }

    block_header_t * block = (block_header_t *)block_mem;
    block->size = size;
    block->is_free = 0;
    block->next = NULL;
    block->prev = last;

    if(last != NULL)
    {
        last->next = block;
    }
    return block;
}