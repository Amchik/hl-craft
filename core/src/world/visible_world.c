#include "core/world/visible_world.h"
#include "core/primitives.h"
#include "core/vector.h"
#include <stdbool.h>
#include <stdint.h>

struct VisibleWorld {
    // Vector of const struct ChunkBase *ptr;
    avec_t v;
};

struct VisibleWorld *VisibleWorld_Init() {
    struct VisibleWorld *v = malloc(sizeof *v);
    v->v = avec_new(sizeof(void *) * 8);
    return v;
}
void VisibleWorld_Free(struct VisibleWorld *world) { avec_free(&world->v); }

void VisibleWorld_PushChunk(struct VisibleWorld *restrict world,
                            const struct ChunkBase *restrict chunk) {
    // NOTE: discarding const *restrict here
    avec_push(&world->v, (void *)&chunk, sizeof(void *));
}
void VisibleWorld_PopFarChunks(struct VisibleWorld *world, ivec3_t center_chunk,
                               uint32_t render_distance) {
    // TODO: write it
}

bool VisibleWorld_IsRegionShouldBeLoaded(const struct VisibleWorld *world,
                                         ivec3_t region_pos) {
    return true;
    // TODO: write it
}
const struct ChunkBase *VisibleWorld_FindChunk(const struct VisibleWorld *world,
                                               ivec3_t pos) {
    const int len = world->v.size / sizeof(void *);
    const struct ChunkBase **arr = world->v.ptr;
    for (int i = 0; i < len; ++i) {
        ivec3_t arr_pos = arr[i]->pos;
        if (arr_pos.x == pos.x && arr_pos.y == pos.y && arr_pos.z == pos.z)
            return arr[i];
    }
    return 0;
}
const struct Block *VisibleWorld_GetBlockAt(const struct VisibleWorld *world,
                                            ivec3_t block_pos) {
    ivec3_t relative_pos = {
        .x = block_pos.x % 16, .y = block_pos.y % 16, .z = block_pos.z % 16};
    if (relative_pos.x < 0)
        relative_pos.x += 16;
    if (relative_pos.y < 0)
        relative_pos.y += 16;
    if (relative_pos.z < 0)
        relative_pos.z += 16;
    ivec3_t chunk_pos = {.x = (block_pos.x - relative_pos.x) / 16,
                         .y = (block_pos.y - relative_pos.y) / 16,
                         .z = (block_pos.z - relative_pos.z) / 16};
    const struct ChunkBase *chunk = VisibleWorld_FindChunk(world, chunk_pos);
    if (!chunk)
        return 0;
    return &chunk->data->blocks[relative_pos.x][relative_pos.y][relative_pos.z];
}

bool VisibleWorld_GetNextUnloadedChunk(
    const struct VisibleWorld *restrict world, ivec3_t *pos,
    uint32_t render_distance) {
    // TODO: write it
    return false;
}
