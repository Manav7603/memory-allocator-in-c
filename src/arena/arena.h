#ifndef ARENA_H
#define ARENA_H

#include "../block/block.h"
#include <stddef.h>

INTERNAL block_header_t *request_space(block_header_t *last, size_t size);

#endif