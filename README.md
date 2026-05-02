# 🚀 High-Performance Memory Allocator in C

> An industrial-grade, custom dynamic memory allocator replicating `malloc`, `free`, `calloc`, and `realloc`. Engineered for high performance, minimal fragmentation, and strict security encapsulation.

![C](https://img.shields.io/badge/language-C-blue.svg)
![Build](https://img.shields.io/badge/build-passing-brightgreen.svg)
![Architecture](https://img.shields.io/badge/architecture-x86__64%20%7C%20ARM64-lightgrey.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)

---

## 📖 Table of Contents
1. [Introduction & Philosophy](#-introduction--philosophy)
2. [Core Architecture](#-core-architecture)
3. [Memory Layout & ASCII Diagrams](#-memory-layout)
4. [Installation & Build](#-installation--build)
5. [API Reference](#-api-reference)
6. [Testing & Validation](#-testing--validation)
7. [Security & Encapsulation](#-security--encapsulation)
8. [Roadmap to v2.0](#-roadmap-to-v20)

---

## 🧠 Introduction & Philosophy

Standard textbook implementations of `malloc` teach the basics of pointer arithmetic but fail in production due to system call bottlenecks and massive metadata overhead. 

This project is built differently. It bridges the gap between theoretical memory management and the high-performance techniques used by modern implementations like `jemalloc` and `tcmalloc`. It utilizes **4MB Pre-allocated Arenas** to eliminate kernel context-switching, employs a **Collective Best-Fit** search algorithm to minimize external fragmentation, and enforces strict **Symbol Visibility** to prevent external binary hijacking.

---

## ⚡ Core Architecture

### 1. The Arena Optimization (Killing the `mmap` Bottleneck)
Making a system call to the Linux Kernel (`mmap` or `sbrk`) for every allocation destroys CPU performance due to user-space/kernel-space context switching. 
* **Our Solution:** When a user requests memory, this allocator maps a massive **4 Megabyte Arena** from the OS at once. It then instantly carves out the requested size and leaves the remaining 3.99MB in a fast, user-space linked list. Subsequent allocations require zero system calls.

### 2. Collective Best-Fit Allocation
Instead of stopping at the first available block (First-Fit), the allocator scans the free list to find the *tightest possible fit* for the requested size. This leaves larger contiguous memory blocks intact for future heavy allocations.

### 3. Bidirectional Coalescing & Splinter Prevention
* **Coalescing:** When memory is freed, the allocator instantly looks forward and backward in the heap. If adjacent blocks are also free, they are immediately merged into a single massive block, eliminating external fragmentation.
* **Splinter Prevention:** If splitting a block would result in a leftover fragment too small to hold a header, the allocator intelligently refuses to split, sacrificing a few bytes to maintain heap integrity.

---

## 📐 Memory Layout

The heap is managed as a doubly-linked list of `block_header_t` structures. All user memory is mathematically aligned to strict **8-byte boundaries** to ensure CPU cache line efficiency and prevent hardware alignment exceptions.

### Heap State: Allocated vs Free
```text
Lower Memory Addresses                                               Higher Addresses
+-------------------+-----------------+   +-------------------+-----------------------+
| BLOCK HEADER      | USER PAYLOAD    |   | BLOCK HEADER      | FREE SPACE (Reusable) |
| size: 32 bytes    | (8-byte aligned)|-->| size: 4064 bytes  |                       |
| is_free: 0        |                 |   | is_free: 1        |                       |
| next: pointer     |                 |   | next: NULL        |                       |
| prev: NULL        |                 |   | prev: pointer     |                       |
+-------------------+-----------------+   +-------------------+-----------------------+
```

### The Splitting Process
When a user asks for `32` bytes from a `4MB` free block:
```text
BEFORE SPLIT:
[ Header: 4MB, Free ] [ ----------------------- 4MB Free Space ----------------------- ]

AFTER SPLIT:
[ Header: 32B, Used ] [ 32B Payload ] | [ New Header: 3.99MB, Free ] [ 3.99MB Free Space ]
```

---

## 🛠️ Installation & Build

This project requires a POSIX-compliant environment (Linux, macOS, WSL) and GCC/Clang.
```bash
# Clone the repository
git clone [https://github.com/Manav7603/memory-allocator-in-c](https://github.com/Manav7603/memory-allocator-in-c)
cd memory-allocator-in-c

# Compile the project and tests
make

# Clean build artifacts
make clean
```

---

## 📦 API Reference

To use the allocator, simply include the public header in your C files. **Note:** The internal memory layout is kept strictly opaque for security.
```c
#include "malloc.h"
```

| Function Signature | Description |
| :--- | :--- |
| `void *my_malloc(size_t size);` | Allocates `size` bytes of memory. Guaranteed to be 8-byte aligned. Returns `NULL` if size is 0 or memory is exhausted. |
| `void my_free(void *ptr);` | Safely deallocates memory. Automatically merges adjacent free blocks. Fails silently on `NULL`. |
| `void *my_calloc(size_t n, size_t size);`| Allocates memory for an array of `n` elements of `size` bytes each. The memory is set to zero. Includes strict integer overflow protection. |
| `void *my_realloc(void *ptr, size_t size);`| Resizes a memory block. If the current block is large enough, it splits in-place. Otherwise, it allocates new space, copies the data, and frees the old block. |

---

## 🛡️ Security & Encapsulation

Unlike textbook allocators that expose their metadata structures globally, this project utilizes **Symbol Visibility Attributes** (`__attribute__((visibility("hidden")))`). 

Internal operations like `split_block()`, `coalesce()`, and `request_space()` are completely stripped from the shared library's export table. It is cryptographically impossible for an external binary to link to or hijack the internal heap management functions.

---

## 🧪 Testing & Validation

The repository includes a comprehensive **White-Box Test Suite** that directly imports internal headers to validate metadata integrity, pointer math, and memory safety.
```bash
# Run the core validation suite
make test

# Run the suite through Valgrind for extreme leak detection
make test-valgrind
```

**The test suite automatically verifies:**
1. Strict 8-byte mathematical pointer alignment.
2. Bidirectional block coalescing logic.
3. Block splitting logic and splinter prevention thresholds.
4. Edge cases: `malloc(0)` and integer multiplication overflow in `calloc`.

---

## 🗺️ Roadmap to v2.0

This project is actively evolving from a linear $O(N)$ allocator into a massive $O(1)$ industrial engine.

- [x] **Phase 1:** Modular architecture and strict symbol visibility.
- [x] **Phase 2:** Elimination of syscall bottleneck via 4MB Arenas.
- [ ] **Phase 3:** Segregated Free Lists (Bins) to reduce search time from $O(N)$ to strictly $O(1)$.
- [ ] **Phase 4:** Thread-Safety via `pthread` mutexes and Thread-Local Caches (tcache).
- [ ] **Phase 5:** Implementation of a Radix Tree for massive (multi-megabyte) allocations.
- [ ] **Phase 6:** Support for `madvise(MADV_DONTNEED)` to return dormant physical memory pages back to the OS.