#pragma once

#include "core/vector.h"
#include "core/texture.h"
#include <math.h>

struct R_Triangle {
    vec3_t v[3];
    tvec2_t t[3];
    uint32_t color;
    const struct AbstractTextureRef *texture;
};

struct R_SceneProps {
    uint32_t width, height;
    vec3_t camera;
    float pitch, yaw;
    float fov;
};
struct R_ScenePropsCalculated {
    float sin_pitch, sin_yaw;
    float cos_pitch, cos_yaw;
    float f, tan_half_fov;
    float frac_width_2, frac_height_2;
};

static inline void R_SceneProps_Calculate(struct R_ScenePropsCalculated *calc,
                                          struct R_SceneProps *props) {
    sincosf(props->pitch * M_PI / 180.0, &calc->sin_pitch, &calc->cos_pitch);
    sincosf(props->yaw * M_PI / 180.0, &calc->sin_yaw, &calc->cos_yaw);
    calc->tan_half_fov = tanf(props->fov * M_PI / 180.0 / 2);
    calc->f = (float)props->width / (2 * tanf(props->fov * M_PI / 180.0 / 2));
    calc->frac_width_2 = props->width / 2.0;
    calc->frac_height_2 = props->height / 2.0;
}

static inline vec3_t
R_VecToCameraSpace(vec3_t v, vec3_t cam,
                   const struct R_ScenePropsCalculated *calc) {
    float x0 = v.x - cam.x;
    float y0 = v.y - cam.y;
    float z0 = v.z - cam.z;
    float x1 = x0 * calc->cos_yaw - z0 * calc->sin_yaw;
    float z1 = x0 * calc->sin_yaw + z0 * calc->cos_yaw;
    float y1 = y0 * calc->cos_pitch - z1 * calc->sin_pitch;
    float z2 = y0 * calc->sin_pitch + z1 * calc->cos_pitch;
    return (vec3_t){x1, y1, z2};
}

__attribute__((deprecated(
    "use R_VecToCameraSpace for each array element"))) static inline void
R_Triag_ToCameraSpace(vec3_t vs[restrict static 3], vec3_t cam,
                      const struct R_ScenePropsCalculated *restrict calc) {
    for (int i = 0; i < 3; ++i) {
        vs[i] = R_VecToCameraSpace(vs[i], cam, calc);
    }
}

static inline vec2_t R_Triag_3D_to_2D(vec3_t point,
                                      const struct R_ScenePropsCalculated *calc) {
    vec2_t r;
    r.x = point.x / point.z * calc->f + calc->frac_width_2;
    r.y = calc->frac_height_2 - point.y / point.z * calc->f;
    return r;
}
