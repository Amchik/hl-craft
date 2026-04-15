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

// Dot product of 3D vectors
static inline float vec3_dot(vec3_t a, vec3_t b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

