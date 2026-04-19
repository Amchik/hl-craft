#include "client-render/world.h"
#include "client-render/scene.h"
#include "core/vector.h"
#include "core/world/block.h"
#include "core/world/visible_world.h"

static const ivec3_t POLYS_OFFSETS[6][4] = {
    [BLOCKFACE_TOP] = {{0, 1, 0}, {1, 1, 0}, {1, 1, 1}, {0, 1, 1}},
    [BLOCKFACE_BOTTOM] = {{0, 0, 1}, {1, 0, 1}, {1, 0, 0}, {0, 0, 0}},
    [BLOCKFACE_XP] = {{1, 0, 0}, {1, 0, 1}, {1, 1, 1}, {1, 1, 0}},
    [BLOCKFACE_ZP] = {{1, 0, 1}, {0, 0, 1}, {0, 1, 1}, {1, 1, 1}},
    [BLOCKFACE_XN] = {{0, 0, 1}, {0, 0, 0}, {0, 1, 0}, {0, 1, 1}},
    [BLOCKFACE_ZN] = {{0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0}},
};

void R_Block_MakeFacePolys(struct R_PolyVec *restrict vec, const struct Block *restrict block,
                           enum BlockFace face, ivec3_t block_pos) {
    // я верю в векторизацию
    // NOTE: тут наверное нужно сначала сделать float (vec3_t), а уже затем
    // такой хренью заниматься
    ivec3_t iv0, iv1, iv2, iv3;
    iv0 = ivec3_add(block_pos, POLYS_OFFSETS[face][0]);
    iv1 = ivec3_add(block_pos, POLYS_OFFSETS[face][1]);
    iv2 = ivec3_add(block_pos, POLYS_OFFSETS[face][2]);
    iv3 = ivec3_add(block_pos, POLYS_OFFSETS[face][3]);

    vec3_t v0, v1, v2, v3;
    v0 = XVEC3(vec3_t, iv0);
    v1 = XVEC3(vec3_t, iv1);
    v2 = XVEC3(vec3_t, iv2);
    v3 = XVEC3(vec3_t, iv3);

    tvec2_t uv0 = (tvec2_t){0.0f, 0.0f};
    tvec2_t uv1 = (tvec2_t){1.0f, 0.0f};
    tvec2_t uv2 = (tvec2_t){1.0f, 1.0f};
    tvec2_t uv3 = (tvec2_t){0.0f, 1.0f};

    struct R_Triangle t1 = {0};
    t1.texture = &block->faces->face[face];
    t1.v[0] = v0;
    t1.v[1] = v1;
    t1.v[2] = v2;
    t1.t[0] = uv0;
    t1.t[1] = uv1;
    t1.t[2] = uv2;

    struct R_Triangle t2 = {0};
    t2.texture = &block->faces->face[face];
    t2.v[0] = v0;
    t2.v[1] = v2;
    t2.v[2] = v3;
    t2.t[0] = uv0;
    t2.t[1] = uv2;
    t2.t[2] = uv3;

    R_PolyVec_Push(vec, &t1);
    R_PolyVec_Push(vec, &t2);
}

static const ivec3_t FACE_OFFSET_MAP[6] = {
    [BLOCKFACE_TOP] = {0, 1, 0}, [BLOCKFACE_BOTTOM] = {0, -1, 0},
    [BLOCKFACE_XP] = {1, 0, 0},  [BLOCKFACE_ZP] = {0, 0, 1},
    [BLOCKFACE_XN] = {-1, 0, 0}, [BLOCKFACE_ZN] = {0, 0, -1},
};

#define IN_CHUNK_BOUNDS(w, dr)                                                 \
    !((w == 0 && dr.w == -1) || (w == 15 && dr.w == 1))
#define IN_CHUNK_BOUNDS_XYZ(x, y, z, dr)                                       \
    (IN_CHUNK_BOUNDS(x, dr) && IN_CHUNK_BOUNDS(y, dr) && IN_CHUNK_BOUNDS(z, dr))

void R_ChunkInstance_CalculateMesh(struct R_ChunkInstance *restrict instance,
                                   const struct VisibleWorld *restrict world) {
    instance->mesh->polys.vec.size = 0;
    const ivec3_t chunk_start = ivec3_mul(instance->base->pos, 16);
    for (int x = 0; x < 16; ++x) {
        for (int z = 0; z < 16; ++z) {
            for (int y = 0; y < 16; ++y) {
                struct Block *b = &instance->base->data->blocks[x][y][z];
                if (b->block_type == BLOCKTYPE_AIR)
                    continue;
                for (int i = 0; i < 6; ++i) {
                    ivec3_t dr = FACE_OFFSET_MAP[i];
                    ivec3_t abs_pos = ivec3_add(chunk_start, (ivec3_t){
                                                                 x,
                                                                 y,
                                                                 z,
                                                             });
                    const struct Block *nb;
                    if (IN_CHUNK_BOUNDS_XYZ(x, y, z, dr))
                        nb = &instance->base->data
                                  ->blocks[x + dr.x][y + dr.y][z + dr.z];
                    else
                        nb = VisibleWorld_GetBlockAt(world,
                                                     ivec3_add(abs_pos, dr));
                    if (!nb || nb->block_type != BLOCKTYPE_AIR)
                        continue;
                    R_Block_MakeFacePolys(&instance->mesh->polys, b, i,
                                          abs_pos);
                }
            }
        }
    }
}
