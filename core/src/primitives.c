#include "core/primitives.h"
#include <stdlib.h>
#include <string.h>

avec_t avec_new(size_t cap) {
    avec_t v = {0};
    if (cap == 0)
        return v;
    v.ptr = malloc(cap);
    v.cap = cap;
    v.size = 0;
    return v;
}
void avec_extendsize(avec_t *vec, size_t newsize) {
    if (vec->cap == 0) {
        *vec = avec_new(newsize);
        return;
    }
    vec->ptr = realloc(vec->ptr, newsize);
    vec->cap = newsize;
}
void avec_push(avec_t *vec, void *ptr, size_t size) {
    if (vec->cap == 0)
        avec_extendsize(vec, size * 4);
    else if (vec->size == vec->cap)
        avec_extendsize(vec, vec->cap * 2);
    memcpy(vec->ptr + vec->size, ptr, size);
    vec->size += size;
}
void avec_free(avec_t *vec) {
    if (vec->cap != 0)
        free(vec->ptr);
}
