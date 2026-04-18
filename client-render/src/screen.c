#include "client-render/screen.h"
#include "client-render/scene.h"
#include "core/vector.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define Z_NEAR 0.01f

struct BoundingBox {
    ivec2_t min, max;
};

static inline int32_t i_max(int32_t a, int32_t b) { return a > b ? a : b; };
static inline int32_t i_min(int32_t a, int32_t b) { return a < b ? a : b; };

static inline struct BoundingBox get_bounding_box(vec2_t v0, vec2_t v1,
                                                  vec2_t v2, ivec2_t size) {
    struct BoundingBox r;

    r.min.x = i_max(0, floorf(fminf(fminf(v0.x, v1.x), v2.x)));
    r.min.y = i_max(0, floorf(fminf(fminf(v0.y, v1.y), v2.y)));

    r.max.x = i_min(size.x - 1, ceilf(fmax(fmax(v0.x, v1.x), v2.x)));
    r.max.y = i_min(size.y - 1, ceilf(fmax(fmax(v0.y, v1.y), v2.y)));

    return r;
}

static inline float edge(vec2_t a, vec2_t b, vec2_t c) {
    return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
}

static void draw_line(uint32_t *buffer, int width, int height, int x0, int y0,
                      int x1, int y1, uint32_t color) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);

    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;

    int err = dx - dy;

    while (1) {
        if (x0 >= 0 && x0 < width && y0 >= 0 && y0 < height) {
            buffer[y0 * width + x0] = color;
        }

        if (x0 == x1 && y0 == y1)
            break;

        int e2 = 2 * err;

        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }

        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

// Draw triangle that projected as camera
static void
R_Screen_DrawTriangle_AsCamera(const struct R_Screen *restrict screen,
                               const struct R_Triangle *restrict triangle,
                               vec3_t vpos[static restrict 3],
                               tvec2_t tpos[static restrict 3]) {
    // skip if nothing from fov visible
    // FIXME: rewrite it
    int frustum_culling = 0;
    for (int j = 0; j < 3; ++j) {
        float ztan = vpos[j].z * screen->calc.tan_half_fov;
        float x = vpos[j].x;
        float y = vpos[j].y;
        if (fabsf(x) > ztan || fabsf(y) > ztan)
            frustum_culling += 1;
    }
    if (frustum_culling == 3) {
        // return;
    }

    vec2_t vs2d[3];
    for (int j = 0; j < 3; ++j) {
        vs2d[j] = R_Triag_3D_to_2D(vpos[j], &screen->calc);
    }

    // Draw wireframe because im lazy
    //    draw_line(screen->buffer, screen->width, screen->height, vs2d[0].x,
    //              vs2d[0].y, vs2d[1].x, vs2d[1].y, triangle->color);
    //    draw_line(screen->buffer, screen->width, screen->height, vs2d[1].x,
    //              vs2d[1].y, vs2d[2].x, vs2d[2].y, triangle->color);
    //    draw_line(screen->buffer, screen->width, screen->height, vs2d[2].x,
    //              vs2d[2].y, vs2d[0].x, vs2d[0].y, triangle->color);

    struct BoundingBox bb =
        get_bounding_box(vs2d[0], vs2d[1], vs2d[2],
                         (ivec2_t){.x = screen->width, .y = screen->height});

    float area = edge(vs2d[0], vs2d[1], vs2d[2]);
    if (area <= 0)
        return;

    for (int32_t y = bb.min.y; y <= bb.max.y; ++y) {
        for (int32_t x = bb.min.x; x <= bb.max.x; ++x) {
            vec2_t p = {x, y};
            // FIXME: так как я забыл про существование знака площади и вообще
            // порядка вершин, чтобы отбрасывание было правильным, wX нужно
            // домножить на sign(area). К тому же из-за рандомного порядка,
            // backface culling не будет тут работать. Когда я исправлю эту
            // ситуацию, нужно будет сделать backface culling и убрать деление
            // на area (сделать его после проверки).
            // NOTE: чисто технически, это результат того, что я так и не
            // заботал поверхностные интегралы...
            float w0 = edge(vs2d[1], vs2d[2], p) / area;
            float w1 = edge(vs2d[2], vs2d[0], p) / area;
            float w2 = edge(vs2d[0], vs2d[1], p) / area;
            if (w0 < 0 || w1 < 0 || w2 < 0)
                continue;

            float z0 = vpos[0].z;
            float z1 = vpos[1].z;
            float z2 = vpos[2].z;

            // BUG: благодаря отсутствию такой мелочи как backface culling,
            // можно получить стрёмный баг, когда z-index работает неправильно.
            // Я это пока не хочу фиксить, но как факт, если это сохранится даже
            // с backface culling, то мб исправлю.
            float z_real = w0 * z0 + w1 * z1 + w2 * z2;
            if (z_real >= screen->zbuffer[y * screen->width + x])
                continue;
            screen->zbuffer[y * screen->width + x] = z_real;

            float u0 = tpos[0].u;
            float u1 = tpos[1].u;
            float u2 = tpos[2].u;

            float v0 = tpos[0].v;
            float v1 = tpos[1].v;
            float v2 = tpos[2].v;

            float u = (w0 * u0 / z0 + w1 * u1 / z1 + w2 * u2 / z2) /
                      (w0 * (1 / z0) + w1 * (1 / z1) + w2 * (1 / z2));

            float v = (w0 * v0 / z0 + w1 * v1 / z1 + w2 * v2 / z2) /
                      (w0 * (1 / z0) + w1 * (1 / z1) + w2 * (1 / z2));

            int tx = (int)(u * 16) % 16;
            int ty = (int)(v * 16) % 16;

            screen->buffer[y * screen->width + x] =
                triangle->texture->texture[ty * 16 + tx];
        }
    }
}

static void R_Screen_DrawTriangle(const struct R_Screen *restrict screen,
                                  const struct R_Triangle *restrict triangle) {
    vec3_t vpos[3];
    vpos[0] = R_VecToCameraSpace(triangle->v[0], screen->camera, &screen->calc);
    vpos[1] = R_VecToCameraSpace(triangle->v[1], screen->camera, &screen->calc);
    vpos[2] = R_VecToCameraSpace(triangle->v[2], screen->camera, &screen->calc);
    tvec2_t tpos[3];
    memcpy(tpos, triangle->t, sizeof(tpos));
    float z0 = vpos[0].z;
    float z1 = vpos[1].z;
    float z2 = vpos[2].z;
    int invisible = (z0 < Z_NEAR) + (z1 < Z_NEAR) + (z2 < Z_NEAR);
    // drop if nothing is visible
    if (invisible == 3)
        return;

    // clip
    else if (invisible == 2) {
        int vis_idx = 0;
        for (; vis_idx < 3; ++vis_idx)
            if (vpos[vis_idx].z >= Z_NEAR)
                break;
        vec3_t vis = vpos[vis_idx];
        for (int j = 0; j < 3; ++j) {
            if (j == vis_idx)
                continue;
            vec3_t b = vpos[j];
            float t = (Z_NEAR - vis.z) / (b.z - vis.z);

            b.x = vis.x + t * (b.x - vis.x);
            b.y = vis.y + t * (b.y - vis.y);
            b.z = Z_NEAR;

            vpos[j] = b;

            tpos[j].u = tpos[vis_idx].u + t * (tpos[j].u - tpos[vis_idx].u);
            tpos[j].v = tpos[vis_idx].v + t * (tpos[j].v - tpos[vis_idx].v);
        }
    }

    // draw two triangles
    else if (invisible == 1) {
        int invis_idx = 0;
        for (; invis_idx < 3; ++invis_idx)
            if (vpos[invis_idx].z < Z_NEAR)
                break;
        vec3_t invis = vpos[invis_idx];
        tvec2_t tinvis = tpos[invis_idx];

        int j = invis_idx == 0 ? 1 : 0;
        vec3_t a = vpos[j];
        float t = (Z_NEAR - a.z) / (invis.z - a.z);
        vec3_t p1;
        p1.x = a.x + t * (invis.x - a.x);
        p1.y = a.y + t * (invis.y - a.y);
        p1.z = Z_NEAR;
        vpos[invis_idx] = p1;
        tpos[invis_idx].u = tpos[j].u + t * (tinvis.u - tpos[j].u);
        tpos[invis_idx].v = tpos[j].v + t * (tinvis.v - tpos[j].v);
        // ^ this triangle save for standard render

        // create triangle (B, P2, P1) and render it:
        int k = j + 1;
        if (k == invis_idx)
            k += 1;
        vec3_t b = vpos[k];
        tvec2_t tb = tpos[k];

        struct R_Triangle other;
        memcpy(&other, triangle, sizeof(*triangle));
        // Debug wireframe
        // other.color = 0xFF0000;
        // triangle->color = 0x00FF00;
        vec3_t o_vpos[3];
        tvec2_t o_tpos[3];
        memcpy(o_vpos, vpos, sizeof(vpos));
        memcpy(o_tpos, tpos, sizeof(tpos));

        t = (Z_NEAR - b.z) / (invis.z - b.z);
        vec3_t p2;
        p2.x = b.x + t * (invis.x - b.x);
        p2.y = b.y + t * (invis.y - b.y);
        p2.z = Z_NEAR;
        o_vpos[j] = b;
        o_vpos[k] = p2; // <-- now it's (b, p2, p1)
        o_tpos[j] = o_tpos[k];
        o_tpos[k].u = tb.u + t * (tinvis.u - tb.u);
        o_tpos[k].v = tb.v + t * (tinvis.v - tb.v);

        // Draw other triangle
        R_Screen_DrawTriangle_AsCamera(screen, &other, o_vpos, o_tpos);
    }

    R_Screen_DrawTriangle_AsCamera(screen, triangle, vpos, tpos);
}

void R_Screen_DrawTriangles(const struct R_Screen *restrict screen,
                            const struct R_Triangle *restrict triangles,
                            size_t count) {
    if (count == 1) {
        R_Screen_DrawTriangle(screen, triangles);
        return;
    }

    for (size_t i = 0; i < count; ++i) {
        R_Screen_DrawTriangle(screen, &triangles[i]);
    }
}
