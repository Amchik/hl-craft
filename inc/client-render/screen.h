#pragma once

#include "client-render/scene.h"
#include <stddef.h>
#include <stdint.h>

struct R_Screen {
    uint32_t width, height;
    uint32_t *buffer;
    float *zbuffer;
    vec3_t camera;
    float pitch, yaw;
    struct R_ScenePropsCalculated calc;
};

static inline void R_Screen_FromSceneProps(struct R_Screen *screen,
                                           struct R_SceneProps *props,
                                           uint32_t *buffer, float *zbuffer) {
    R_SceneProps_Calculate(&screen->calc, props);
    screen->buffer = buffer;
    screen->zbuffer = zbuffer;
    screen->pitch = props->pitch;
    screen->yaw = props->yaw;
    screen->width = props->width;
    screen->height = props->height;
    screen->camera = props->camera;
}

void R_Screen_DrawTriangles(struct R_Screen *screen,
                            struct R_Triangle *triangles, size_t count);
