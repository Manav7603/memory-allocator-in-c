# Memory Allocator in C

A custom dynamic memory allocator in C, replicating `malloc`, `free`, `calloc`, and `realloc` with a linked-list heap manager, block splitting, and coalescing.

## Quick Start

```bash
git clone https://github.com/Manav7603/memory-allocator-in-c
cd memory-allocator-in-c
```

```bash
# Compile
make

# Run tests
make test

# Clean
make clean
```

**Available commands**: `make`, `make test`, `make test-valgrind`, `make clean`, `make help`

## Features

- **Custom malloc**: Allocates memory blocks from the heap with 8-byte alignment
- **Custom free**: Deallocates memory and returns it to the free pool
- **Block splitting**: Splits large free blocks to minimize internal fragmentation
- **Block coalescing**: Merges adjacent free blocks to reduce external fragmentation
- **Custom calloc**: Allocates zeroed-out memory
- **Custom realloc**: Resizes existing memory blocks
- **First-fit strategy**: Reuses free blocks in a simple, efficient manner

## Project Structure

```
.
├── Makefile                    # Build configuration
├── README.md                   # This file
├── src/
│   ├── malloc.c               # Implementation of memory allocator
│   └── malloc.h               # Public API header
└── tests/
    ├── test_split_coalesce.c  # Unit tests for split/coalesce functionality
    └── sample_c.c             # Sample C program
```

## API Reference

#### Block Header
```c
typedef struct block_header {
    size_t size;                   // Size of user data
    int is_free;                   // 1 = free, 0 = allocated
    struct block_header *next;     // Next block
    struct block_header *prev;     // Previous block
} block_header_t;
```

#### Functions

- **`void *my_malloc(size_t size)`** — Allocates memory with 8-byte alignment, reuses free blocks (first-fit), splits large blocks
- **`void my_free(void *ptr)`** — Deallocates memory, marks as free, coalesces adjacent free blocks
- **`void *my_calloc(size_t nelem, size_t elem_size)`** — Allocates and zeros memory, prevents overflow
- **`void *my_realloc(void *ptr, size_t size)`** — Resizes blocks in-place or allocates new space

## Implementation

### Core Strategy

- **Heap Layout**: Doubly-linked list of blocks with headers
- **Memory Reuse**: First-fit search for suitable free blocks
- **Block Splitting**: Splits large blocks only if remainder ≥ 16 bytes (header + MIN_SPLIT_SIZE)
- **Coalescing**: Merges adjacent free blocks immediately upon deallocation

### Key Functions

- `align8()` — Aligns sizes to 8-byte boundaries for cache efficiency
- `find_free_block()` — First-fit search through linked list
- `request_space()` — Requests heap memory from OS via `sbrk()`
- `split_block()` — Splits free block, prevents splinters
- `coalesce()` — Merges adjacent free blocks (forward, backward, and forward again)

### Memory Layout
```
[Header][Data] [Header][Data] [Header][Free Space]
         ↑                                   ↑
      Allocated                         Reusable
```

When splitting: `[Header][Allocated] [New Header][Free Remainder]`

## Tests

[test_split_coalesce.c](tests/test_split_coalesce.c) validates core functionality:
- Allocates three adjacent blocks and verifies they're independent
- Frees adjacent blocks and verifies automatic coalescing
- Allocates large block in coalesced space to test reuse
- Allocates small block to test splitting into allocated + free portions
- Validates all block headers (size, free flags, pointers)

Run with: `make test`

## Limitations

- Single-threaded (not thread-safe)
- First-fit strategy (simple, not optimized)
- Memory can only grow (OS manages heap), no shrinking
- No custom memory pool support

## Future Improvements

- Thread-safe implementation with locks
- Best-fit allocation strategy
- Buddy system allocation
- Heap introspection API
- Performance benchmarking suite
