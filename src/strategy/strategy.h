#ifndef STRATEGY_H
#define STRATEGY_H

#include "../block/block.h"
#include <stddef.h>

INTERNAL block_header_t *find_free_block(size_t size);

#endif