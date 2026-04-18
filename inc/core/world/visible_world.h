#pragma once

#include "core/vector.h"
#include "core/world/block.h"
#include "core/world/chunk.h"
#include <stdbool.h>

struct VisibleWorld;

struct VisibleWorld *VisibleWorld_Init();
void VisibleWorld_Free(struct VisibleWorld *world);

void VisibleWorld_PushChunk(struct VisibleWorld *restrict world,
                            const struct ChunkBase *restrict chunk);
void VisibleWorld_PopFarChunks(struct VisibleWorld *world, ivec3_t center_chunk,
                               uint32_t render_distance);

bool VisibleWorld_IsRegionShouldBeLoaded(const struct VisibleWorld *world,
                                         ivec3_t region_pos);

const struct ChunkBase *VisibleWorld_FindChunk(const struct VisibleWorld *world,
                                               ivec3_t pos);
const struct Block *VisibleWorld_GetBlockAt(const struct VisibleWorld *world,
                                            ivec3_t pos);

bool VisibleWorld_GetNextUnloadedChunk(
    const struct VisibleWorld *restrict world, ivec3_t *pos,
    uint32_t render_distance);
