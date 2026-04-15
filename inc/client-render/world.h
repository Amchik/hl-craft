#pragma once

#include "client-render/containers/triangle.h"
#include "client-render/texture.h"
#include "core/vector.h"

enum R_BlockType {
    R_BTYPE_AIR,
    R_BTYPE_SOLID,
};

struct R_Block {
    enum R_BlockType block_type;
    struct R_BlockFaces *faces;
};

struct R_Chunk {
    // Some big data that requires heap allocation
    struct {
        // [x*16 + z][y]
        struct R_Block blocks[256][16];
    } *data;
    ivec3_t pos;
};

void R_Block_MakeFacePolys(struct R_PolyVec *vec, struct R_Block *block,
                           enum R_Face face, ivec3_t block_pos);
