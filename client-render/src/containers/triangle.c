#include "client-render/containers/triangle.h"
#include "client-render/scene.h"
#include "core/primitives.h"

struct R_PolyVec R_PolyVec_Init() {
    struct R_PolyVec v;
    v.vec = avec_new(sizeof(struct R_Triangle) * 8);
    return v;
}
struct R_PolyVec R_PolyVec_InitWithCapacity(size_t cap) {
    struct R_PolyVec v;
    v.vec = avec_new(sizeof(struct R_Triangle) * cap);
    return v;
}
void R_PolyVec_Push(struct R_PolyVec *vec, struct R_Triangle *triangle) {
    avec_push(&vec->vec, triangle, sizeof(*triangle));
}
