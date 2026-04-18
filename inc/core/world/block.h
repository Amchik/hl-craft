#pragma once

#include "core/texture.h"

enum BlockFace {
    BLOCKFACE_TOP = 0,
    BLOCKFACE_BOTTOM = 1,
    // positive/negative x/z
    BLOCKFACE_XP = 2,
    BLOCKFACE_ZP = 3,
    BLOCKFACE_XN = 4,
    BLOCKFACE_ZN = 5,
};

struct BlockFaces {
    // Face texture indexed by `BlockFace` enum
    struct AbstractTextureRef face[6];
};

enum BlockType {
    BLOCKTYPE_AIR,
    BLOCKTYPE_SOLID,
};

struct Block {
    enum BlockType block_type;
    // Faces of block. If `block_type` is air, this pointer will be NULL.
    const struct BlockFaces *faces;
};
