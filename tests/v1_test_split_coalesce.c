#include <assert.h>
#include <stdio.h>
#include "malloc.h"

int main(void)
{
    printf("\n🧪 Memory Allocator Test Suite\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n");

    /*
     * Create three blocks.
     * They should be adjacent in the heap.
     */
    printf("Test 1: Allocating three 64-byte blocks...\n");
    void *a = my_malloc(64);
    void *b = my_malloc(64);
    void *c = my_malloc(64);

    assert(a != NULL);
    assert(b != NULL);
    assert(c != NULL);
    printf("  ✓ Allocated: a=%p, b=%p, c=%p\n\n", a, b, c);

    /*
     * Free the middle and right blocks.
     * Since they are adjacent and both free, they should merge.
     */
    printf("Test 2: Freeing middle (b) and right (c) blocks...\n");
    my_free(b);
    my_free(c);
    printf("  ✓ Blocks freed and coalesced\n\n");

    /*
     * Now ask for a block large enough to use the merged space.
     * The allocator should reuse the merged block starting at b.
     */
    printf("Test 3: Allocating 120-byte block (should reuse merged space)...\n");
    void *big = my_malloc(120);
    assert(big == b);
    printf("  ✓ Reused block at original b location: %p\n\n", big);

    /*
     * Free it again so we can test splitting.
     */
    printf("Test 4: Freeing 120-byte block...\n");
    my_free(big);
    printf("  ✓ Block freed\n\n");

    /*
     * Ask for a much smaller block.
     * The allocator should split the free block into:
     * - one allocated 16-byte block
     * - one leftover free block
     */
    printf("Test 5: Allocating 16-byte block (should split)...\n");
    void *small = my_malloc(16);
    assert(small == big);
    printf("  ✓ Block split successfully: small=%p\n", small);

    block_header_t *hdr = ((block_header_t *)small) - 1;
    assert(hdr->size == 16);
    assert(hdr->next != NULL);
    assert(hdr->next->is_free == 1);
    printf("  ✓ Allocated portion: 16 bytes\n");
    printf("  ✓ Remaining free portion: %zu bytes\n\n", hdr->next->size);

    printf("Test 6: Cleanup - freeing all remaining blocks...\n");
    my_free(small);
    my_free(a);
    printf("  ✓ All memory freed successfully\n\n");

    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("✓ All tests passed!\n\n");
    return 0;
}