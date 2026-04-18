#pragma once

#include "core/vector.h"
#include "core/world/block.h"

struct ChunkBase {
    // Some big data that requires heap allocation
    struct {
        // [x][y][z]
        struct Block blocks[16][16][16];
    } *data;
    ivec3_t pos;
};
