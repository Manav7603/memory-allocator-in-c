#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

// Include public API
#include "malloc.h" 
// Include INTERNAL API to verify block metadata (White-Box Testing)
#include "block/block.h" 

int main(void)
{
    printf("\n🧪 Memory Allocator Test Suite\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n");

    /* ---------------------------------------------------------
     * TEST 1: Basic Allocation & Alignment
     * --------------------------------------------------------- */
    printf("Test 1: Allocating three 64-byte blocks...\n");
    void *a = my_malloc(64);
    void *b = my_malloc(64);
    void *c = my_malloc(64);

    assert(a != NULL);
    assert(b != NULL);
    assert(c != NULL);
    
    // Verify 8-byte alignment using bitwise AND
    assert(((uintptr_t)a & 7) == 0); 
    
    printf("  ✓ Allocated: a=%p, b=%p, c=%p\n", a, b, c);
    printf("  ✓ Memory addresses are properly 8-byte aligned\n\n");

    /* ---------------------------------------------------------
     * TEST 2: Coalescing (Merging Free Blocks)
     * --------------------------------------------------------- */
    printf("Test 2: Freeing middle (b) and right (c) blocks...\n");
    my_free(b);
    my_free(c);
    
    // b and c should now be merged into one giant block
    block_header_t *b_hdr = ((block_header_t *)b) - 1;
    assert(b_hdr->is_free == 1);
    printf("  ✓ Blocks freed and coalesced\n\n");

    /* ---------------------------------------------------------
     * TEST 3: Best-Fit / First-Fit Reuse
     * --------------------------------------------------------- */
    printf("Test 3: Allocating 120-byte block (should reuse merged space)...\n");
    void *big = my_malloc(120);
    // Because b and c merged, 'big' should perfectly slot into the old 'b' address
    assert(big == b); 
    printf("  ✓ Reused block at original b location: %p\n\n", big);

    printf("Test 4: Freeing 120-byte block...\n");
    my_free(big);
    printf("  ✓ Block freed\n\n");

    /* ---------------------------------------------------------
     * TEST 4: Splitting Blocks
     * --------------------------------------------------------- */
    printf("Test 5: Allocating 16-byte block (should split)...\n");
    void *small = my_malloc(16);
    assert(small == big);
    
    block_header_t *hdr = ((block_header_t *)small) - 1;
    assert(hdr->size == 16);
    assert(hdr->next != NULL);
    assert(hdr->next->is_free == 1);
    
    printf("  ✓ Block split successfully: small=%p\n", small);
    printf("  ✓ Allocated portion: 16 bytes\n");
    printf("  ✓ Remaining free portion: %zu bytes\n\n", hdr->next->size);

    /* ---------------------------------------------------------
     * TEST 5: Edge Cases (0 Bytes & Calloc)
     * --------------------------------------------------------- */
    printf("Test 6: Edge Cases...\n");
    void *zero_ptr = my_malloc(0);
    assert(zero_ptr == NULL);
    printf("  ✓ malloc(0) correctly returns NULL\n");

    int *arr = (int *)my_calloc(5, sizeof(int));
    assert(arr != NULL);
    assert(arr[0] == 0 && arr[4] == 0); // Verify it is zeroed out
    printf("  ✓ calloc zeros out memory correctly\n\n");

    /* ---------------------------------------------------------
     * CLEANUP
     * --------------------------------------------------------- */
    printf("Test 7: Cleanup - freeing all remaining blocks...\n");
    my_free(small);
    my_free(a);
    my_free(arr);
    printf("  ✓ All memory freed successfully\n\n");

    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("🏆 All tests passed successfully!\n\n");
    return 0;
}