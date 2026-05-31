#pragma once
#include <stdint.h>

static inline uint32_t xorshift32(uint32_t state) {
    uint32_t x = state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return x;
}
