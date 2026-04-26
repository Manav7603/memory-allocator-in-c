#include <assert.h>
#include <stdio.h>
#include "malloc.h"

int main(void)
{
    /*
     * Create three blocks.
     * They should be adjacent in the heap.
     */
    void *a = my_malloc(64);
    void *b = my_malloc(64);
    void *c = my_malloc(64);

    assert(a != NULL);
    assert(b != NULL);
    assert(c != NULL);

    /*
     * Free the middle and right blocks.
     * Since they are adjacent and both free, they should merge.
     */
    my_free(b);
    my_free(c);

    /*
     * Now ask for a block large enough to use the merged space.
     * The allocator should reuse the merged block starting at b.
     */
    void *big = my_malloc(120);
    assert(big == b);

    /*
     * Free it again so we can test splitting.
     */
    my_free(big);

    /*
     * Ask for a much smaller block.
     * The allocator should split the free block into:
     * - one allocated 16-byte block
     * - one leftover free block
     */
    void *small = my_malloc(16);
    assert(small == big);

    block_header_t *hdr = ((block_header_t *)small) - 1;
    assert(hdr->size == 16);
    assert(hdr->next != NULL);
    assert(hdr->next->is_free == 1);

    my_free(small);
    my_free(a);

    printf("split/coalesce test passed\n");
    return 0;
}