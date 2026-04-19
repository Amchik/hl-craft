#pragma once

#include "client-render/containers/triangle.h"
#include "core/vector.h"
#include "core/world/block.h"
#include "core/world/chunk.h"
#include "core/world/visible_world.h"
#include <stdbool.h>

struct R_ChunkMesh {
    struct R_PolyVec polys;

    bool dirty;
};

struct R_ChunkInstance {
    const struct ChunkBase *base;
    struct R_ChunkMesh *mesh;
};

void R_Block_MakeFacePolys(struct R_PolyVec *restrict vec,
                           const struct Block *restrict block, enum BlockFace face,
                           ivec3_t block_pos);

void R_ChunkInstance_CalculateMesh(struct R_ChunkInstance *restrict instance,
                                   const struct VisibleWorld *restrict world);
