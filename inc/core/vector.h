#pragma once

#include <stdint.h>

typedef struct {
    float x, y, z;
} vec3_t;

typedef struct {
    float x, y;
} vec2_t;

typedef struct {
    float u, v;
} tvec2_t;

typedef struct {
    uint8_t x, y, z;
} bvec3_t;

typedef struct {
    int32_t x, y;
} ivec2_t;

typedef struct {
    int32_t x, y, z;
} ivec3_t;

// Construct *vec3_t from any other *vec3_t with implicit cast
#define XVEC3(ty, from) ((ty){(from).x, (from.y), (from).z})

static inline vec3_t vec3_add(vec3_t a, vec3_t b) {
    return (vec3_t){a.x + b.x, a.y + b.y, a.z + b.z};
}

static inline ivec3_t ivec3_add(ivec3_t a, ivec3_t b) {
    return (ivec3_t){a.x + b.x, a.y + b.y, a.z + b.z};
}

static inline ivec3_t ivec3_mul(ivec3_t a, int32_t k) {
    return (ivec3_t){k * a.x, k * a.y, k * a.z};
}
