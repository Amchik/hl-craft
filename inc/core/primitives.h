#pragma once

#include <stdlib.h>

typedef struct {
    void *ptr;
    size_t size;
    size_t cap;
} avec_t;

avec_t avec_new(size_t cap);
void avec_push(avec_t *vec, void *ptr, size_t size);
void avec_extendsize(avec_t *vec, size_t newsize);
void avec_free(avec_t *vec);
