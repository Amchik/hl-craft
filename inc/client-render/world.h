#pragma once

#include "client-render/texture.h"
#include "core/vector.h"

enum R_BlockType {
    R_BTYPE_AIR,
    R_BTYPE_SOLID,
};

struct R_Block {
    enum R_BlockType block_type;
    struct R_BlockFaces *faces;
    // Chunk-relative position
    //bvec3_t pos;
};

enum R_ChunkFaces {
    R_CFACE_TOP = 1,
    R_CFACE_BOTTOM = 1 << 1,
    // positive/negative x/z
    R_CFACE_PP = 1 << 2,
    R_CFACE_PN = 1 << 3,
    R_CFACE_NP = 1 << 4,
    R_CFACE_NN = 1 << 5,
};

struct R_Chunk {
    struct R_Block blocks[256];
};

