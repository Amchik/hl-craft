#include "client-render/scene.h"
#include "core/primitives.h"
#include <stddef.h>

struct R_PolyVec {
    avec_t vec;
};

struct R_PolyVec R_PolyVec_Init();
struct R_PolyVec R_PolyVec_InitWithCapacity(size_t cap);
void R_PolyVec_Push(struct R_PolyVec *vec, struct R_Triangle *triangle);

static inline size_t R_PolyVec_Len(struct R_PolyVec *vec) {
    return vec->vec.size / sizeof(struct R_Triangle);
}

