#include "client-render/scene.h"
#include "client-render/screen.h"
#include "client-render/world.h"
#include "core/texture.h"
#include "core/vector.h"
#include "core/world/chunk.h"
#include "core/world/visible_world.h"
#include "input-helper.h"
#include "xdg-shell-client-protocol.h"
#include <X11/keysymdef.h>
#include <cairo/cairo.h>
#include <fcntl.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>
#include <wayland-client-core.h>
#include <wayland-client.h>
#include <wayland-util.h>
#include <xkbcommon/xkbcommon-keysyms.h>
#include <xkbcommon/xkbcommon.h>

#define WIDTH 800
#define HEIGHT 600

uint32_t gravel[] = {
    0x838383, 0xBEBEBE, 0x8C8C8C, 0xADADAD, 0xB2B2B2, 0xC2C2C2, 0x9F9F9F,
    0xC3C3C3, 0x9F9F9F, 0xBDBDBD, 0xAEAEAE, 0xAAAAAA, 0x969696, 0xAEAEAE,
    0x8E8E8E, 0xA1A1A1, 0x8D8D8D, 0x9C9C9C, 0x9D9D9D, 0xABABAB, 0x8E8E8E,
    0x8B8B8B, 0xB9B9B9, 0x949494, 0xB4B4B4, 0x919191, 0xBDBDBD, 0xB6B6B6,
    0xAFAFAF, 0x8B8B8B, 0x797979, 0xA1A1A1, 0x9A9A9A, 0xA7A7A7, 0xA5A5A5,
    0x848484, 0x757575, 0xA6A6A6, 0xB8B8B8, 0x848484, 0x949494, 0x8C8C8C,
    0xB4B4B4, 0xB4B4B4, 0x9B9B9B, 0x8D8D8D, 0x979797, 0x9B9B9B, 0xA0A0A0,
    0x9F9F9F, 0x787878, 0xA0A0A0, 0x9D9D9D, 0xCACACA, 0x999999, 0xB3B3B3,
    0xA9A9A9, 0xA8A8A8, 0xA1A1A1, 0xB2B2B2, 0xB5B5B5, 0xA2A2A2, 0xA8A8A8,
    0xA4A4A4, 0xC3C3C3, 0xA5A5A5, 0xA1A1A1, 0xB8B8B8, 0xADADAD, 0xA7A7A7,
    0x909090, 0xB3B3B3, 0xCCCCCC, 0xBCBCBC, 0x9A9A9A, 0x868686, 0x7E7E7E,
    0xA6A6A6, 0xABABAB, 0xB3B3B3, 0x9F9F9F, 0x939393, 0x9D9D9D, 0xA5A5A5,
    0x999999, 0x959595, 0xBBBBBB, 0xA6A6A6, 0xAFAFAF, 0xA6A6A6, 0xADADAD,
    0x949494, 0x959595, 0xA8A8A8, 0x9E9E9E, 0xA7A7A7, 0x707070, 0xBDBDBD,
    0x959595, 0x7F7F7F, 0x959595, 0xB2B2B2, 0xA4A4A4, 0x7F7F7F, 0x787878,
    0xA3A3A3, 0xB9B9B9, 0xB9B9B9, 0xCCCCCC, 0x8D8D8D, 0xA7A7A7, 0xBEBEBE,
    0x8D8D8D, 0x696969, 0xB0B0B0, 0x808080, 0xBDBDBD, 0xA8A8A8, 0xB5B5B5,
    0xC4C4C4, 0xBABABA, 0x959595, 0x9A9A9A, 0x878787, 0xA8A8A8, 0xAAAAAA,
    0xBBBBBB, 0xBABABA, 0xCECECE, 0xB7B7B7, 0xBEBEBE, 0x8E8E8E, 0xBEBEBE,
    0x9B9B9B, 0x969696, 0xA1A1A1, 0xAAAAAA, 0x8C8C8C, 0xB5B5B5, 0x999999,
    0x9E9E9E, 0x949494, 0xA5A5A5, 0x676767, 0x969696, 0x9C9C9C, 0xADADAD,
    0xA3A3A3, 0x6E6E6E, 0xA6A6A6, 0xD6D6D6, 0x9A9A9A, 0xACACAC, 0xA2A2A2,
    0x878787, 0xCDCDCD, 0xBEBEBE, 0xC8C8C8, 0xA3A3A3, 0x9D9D9D, 0xBBBBBB,
    0xBABABA, 0xBBBBBB, 0x9B9B9B, 0x787878, 0x9B9B9B, 0xA0A0A0, 0xA7A7A7,
    0xB0B0B0, 0xABABAB, 0xBCBCBC, 0x909090, 0x7C7C7C, 0xB3B3B3, 0x909090,
    0x949494, 0x9A9A9A, 0x717171, 0xB4B4B4, 0xA5A5A5, 0x979797, 0xB1B1B1,
    0xA6A6A6, 0x868686, 0x8F8F8F, 0x9D9D9D, 0x949494, 0x838383, 0xA7A7A7,
    0x717171, 0xA0A0A0, 0xC1C1C1, 0xB3B3B3, 0xAAAAAA, 0xA4A4A4, 0x9B9B9B,
    0xB7B7B7, 0xA3A3A3, 0x8D8D8D, 0xAEAEAE, 0xB1B1B1, 0xB9B9B9, 0xABABAB,
    0xA8A8A8, 0xADADAD, 0x9E9E9E, 0xB7B7B7, 0xB0B0B0, 0xBCBCBC, 0xA6A6A6,
    0xC1C1C1, 0x9F9F9F, 0x686868, 0xA8A8A8, 0xB6B6B6, 0x9D9D9D, 0xB8B8B8,
    0xB0B0B0, 0xADADAD, 0x8C8C8C, 0xB6B6B6, 0xC0C0C0, 0xC7C7C7, 0xD4D4D4,
    0x898989, 0x959595, 0xAAAAAA, 0x969696, 0xA2A2A2, 0xA9A9A9, 0xA8A8A8,
    0xBEBEBE, 0xA0A0A0, 0x9C9C9C, 0xBEBEBE, 0xB0B0B0, 0x9C9C9C, 0xA8A8A8,
    0xB9B9B9, 0xB2B2B2, 0xB7B7B7, 0xB6B6B6, 0x8E8E8E, 0x9F9F9F, 0xC6C6C6,
    0x9F9F9F, 0x838383, 0x909090, 0xC7C7C7, 0x7B7B7B, 0xC7C7C7, 0x9D9D9D,
    0x959595, 0x979797, 0x999999, 0xBFBFBF,
};
struct AbstractTextureRef texture_gravel = {
    ._texture = {._ty_16x16 = {.texture = gravel}},
    ._texture_ty = TEXTURE_TYPE_16X16};
struct BlockFaces gravel_block_faces;
struct Block gravel_block = {
    .block_type = BLOCKTYPE_SOLID,
    .faces = &gravel_block_faces,
};

#define CHUNKS_COUNT 2
struct R_ChunkInstance CHUNKS[CHUNKS_COUNT];

struct VisibleWorld *WORLD;

struct client_state {
    struct wl_display *display;
    struct wl_registry *registry;
    struct wl_compositor *compositor;
    struct wl_shm *shm;
    struct xdg_wm_base *wm_base;
    struct wl_seat *wl_seat;

    struct wl_surface *surface;
    struct xdg_surface *xdg_surface;
    struct xdg_toplevel *xdg_toplevel;

    struct wl_buffer *buffer;
    uint32_t *shm_buffer;

    struct wl_callback *frame_callback;

    bool configured;
    uint64_t render_since;
};

__attribute__((format(printf, 5, 6))) void
draw_text(void *buffer, int line, int width, int height, char *fmt, ...) {
    line++;
    cairo_surface_t *surface = cairo_image_surface_create_for_data(
        buffer, CAIRO_FORMAT_ARGB32, width, height, width * 4);

    cairo_t *cr = cairo_create(surface);

    cairo_set_operator(cr, CAIRO_OPERATOR_OVER);

    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);

    cairo_select_font_face(cr, "monospace", CAIRO_FONT_SLANT_NORMAL,
                           CAIRO_FONT_WEIGHT_NORMAL);

    cairo_set_font_size(cr, 12);

    cairo_move_to(cr, 5, 20 * line);

    char buff[256];
    va_list va;
    va_start(va, fmt);
    vsnprintf(buff, 255, fmt, va);
    buff[255] = 0;

    cairo_show_text(cr, buff);

    cairo_destroy(cr);
    cairo_surface_destroy(surface);
}

uint64_t current_time_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (uint64_t)(ts.tv_sec) * 1000 + ts.tv_nsec / 1000000;
}

#define TRIAG_CNT 6

struct R_Triangle TRIANGLES[TRIAG_CNT + 0] = {
    {.v = {{0, 0, 0}, {1, 0, 0}, {1, 0, 1}}, .t = {{0, 0}, {1, 0}, {1, 1}}},
    {.v = {{0, 0, 0}, {1, 0, 1}, {0, 0, 1}}, .t = {{0, 0}, {1, 1}, {0, 1}}},
    {.v = {{0, 0, 0}, {0, -1, 0}, {1, -1, 0}}, .t = {{0, 0}, {0, 1}, {1, 1}}},
    {.v = {{0, 0, 0}, {1, -1, 0}, {1, 0, 0}}, .t = {{0, 0}, {1, 1}, {0, 1}}},
    {.v = {{0, 0, 0}, {0, 0, 1}, {0, -1, 1}}, .t = {{0, 0}, {1, 0}, {1, 1}}},
    {.v = {{0, 0, 0}, {0, -1, 1}, {0, -1, 0}}, .t = {{0, 0}, {1, 1}, {0, 1}}}};

#define Z_NEAR 0.01f

#define DEF_CAMERA_POS {0, 1, -3}
struct R_SceneProps sprops = {.width = WIDTH,
                              .height = HEIGHT,
                              .camera = DEF_CAMERA_POS,
                              .pitch = 0,
                              .yaw = 0,
                              .fov = 60};
uint32_t *work_buffer;
float *z_buffer;
float *z_buffer_INF;
vec2_t debug_move = {0};
vec2_t debug_rotate = {0};

#define MAX(a, b) (a) > (b) ? (a) : (b)
#define MIN(a, b) (a) < (b) ? (a) : (b)

void move_forward(float len) {
    float cp = cosf(sprops.pitch * M_PI / 180);
    float sp = sinf(sprops.pitch * M_PI / 180);
    float cy = cosf(sprops.yaw * M_PI / 180);
    float sy = sinf(sprops.yaw * M_PI / 180);

    vec3_t forward;
    forward.x = sy * cp;
    forward.y = sp;
    forward.z = cy * cp;

    sprops.camera.x += forward.x * len;
    sprops.camera.y += forward.y * len;
    sprops.camera.z += forward.z * len;
}

void move_right(float len) {
    float cy = cosf(sprops.yaw * M_PI / 180);
    float sy = sinf(sprops.yaw * M_PI / 180);

    vec3_t right;
    right.x = cy;
    right.y = 0.0f;
    right.z = -sy;

    sprops.camera.x += right.x * len;
    sprops.camera.y += right.y * len;
    sprops.camera.z += right.z * len;
}

static void frame_done(void *data, struct wl_callback *cb, uint32_t time) {
    struct client_state *state = data;
    wl_callback_destroy(cb);
    state->frame_callback = NULL;

    uint64_t now = current_time_ms();
    uint64_t frametime = now - state->render_since;

    // Request next frame
    state->frame_callback = wl_surface_frame(state->surface);
    static const struct wl_callback_listener listener = {.done = frame_done};
    wl_callback_add_listener(state->frame_callback, &listener, state);

    if (frametime < (1000 / 60)) {
        wl_surface_commit(state->surface);
        return;
    }

    if (debug_rotate.x != 0) {
        sprops.pitch += debug_rotate.x;
        if (sprops.pitch < -90)
            sprops.pitch = -90.;
        else if (sprops.pitch > 90)
            sprops.pitch = 90.;
    }
    if (debug_rotate.y != 0) {
        sprops.yaw += debug_rotate.y;
        if (sprops.yaw > 180)
            sprops.yaw -= 360;
        else if (sprops.yaw < -180)
            sprops.yaw += 360;
    }

    if (debug_move.x != 0) {
        move_forward(debug_move.x);
    }
    if (debug_move.y != 0) {
        move_right(debug_move.y);
    }

    state->render_since = now;
    memset(work_buffer, 0, WIDTH * HEIGHT * 4);
    memcpy(z_buffer, z_buffer_INF, WIDTH * HEIGHT * sizeof(float));

    //    struct R_Triangle triangles[TRIAG_CNT];
    //    memcpy(triangles, TRIANGLES, sizeof(TRIANGLES));
    //    for (int i = 0; i < TRIAG_CNT; ++i) {
    //        triangles[i].color = 0 * 0xFFFFFF;
    //        triangles[i].texture = &texture_gravel;
    //    }

    struct R_Screen screen;
    R_Screen_FromSceneProps(&screen, &sprops, work_buffer, z_buffer);
    for (int i = 0; i < CHUNKS_COUNT; ++i) {
        struct R_PolyVec *polyvec = &CHUNKS[i].mesh->polys;
        R_Screen_DrawTriangles(&screen, polyvec->vec.ptr,
                               R_PolyVec_Len(polyvec));
    }
    // R_Screen_DrawTriangles(&screen, triangles, TRIAG_CNT);
    // R_Screen_DrawTriangles(&screen, &triang, 1);

    draw_text(work_buffer, 0, WIDTH, HEIGHT,
              "x / y / z: %f %f %f | pitch=%f, yaw=%f | fov = %f",
              sprops.camera.x, sprops.camera.y, sprops.camera.z, sprops.pitch,
              sprops.yaw, sprops.fov);
    draw_text(work_buffer, 1, WIDTH, HEIGHT,
              "fps: %f, frametime: %ld ms, *render stats broken*",
              1000.0 / frametime, frametime);
    draw_text(work_buffer, 2, WIDTH, HEIGHT,
              "[controls] WASD/arrows, [1] to reset pos, [0] to set to zero, "
              "[-]/[=] change fov");

    memcpy(state->shm_buffer, work_buffer, WIDTH * HEIGHT * 4);

    wl_surface_attach(state->surface, state->buffer, 0, 0);
    wl_surface_damage(state->surface, 0, 0, WIDTH, HEIGHT);

    wl_surface_commit(state->surface);
}
void on_keyboard_input(void *data, struct wl_keyboard *wl_keyboard,
                       uint32_t serial, uint32_t time, xkb_keysym_t key,
                       uint32_t key_state) {
    struct client_state *state = data;

    if (key_state > 1)
        key_state = 1;

    switch (key) {
    case 'q':
        exit(0);
        break;
    case XKB_KEY_Up:
        debug_rotate.x = 1. * key_state;
        break;
    case XKB_KEY_Down:
        debug_rotate.x = -1. * key_state;
        break;
    case XKB_KEY_Left:
        debug_rotate.y = -1. * key_state;
        break;
    case XKB_KEY_Right:
        debug_rotate.y = 1. * key_state;
        break;
    case 'w':
        debug_move.x = 0.2 * key_state;
        break;
    case 's':
        debug_move.x = -0.2 * key_state;
        break;
    case 'a':
        debug_move.y = -0.2 * key_state;
        break;
    case 'd':
        debug_move.y = 0.2 * key_state;
        break;
    case '0':
        sprops.camera = (vec3_t){0};
        sprops.pitch = sprops.yaw = 0;
        break;
    case '1':
        sprops.camera = (vec3_t)DEF_CAMERA_POS;
        sprops.pitch = sprops.yaw = 0;
        break;
    case '-':
        sprops.fov -= 5;
        break;
    case '=':
        sprops.fov += 5;
        break;
    }
}

int pointer_prev_x = -1, pointer_prev_y = -1;
bool pointer_button_pressed = false;

void on_pointer_motion(void *data, struct wl_pointer *wl_pointer, uint32_t time,
                       wl_fixed_t surface_x, wl_fixed_t surface_y) {
    if (pointer_button_pressed) {
        if (pointer_prev_x == -1) {
            pointer_prev_x = wl_fixed_to_int(surface_x);
            pointer_prev_y = wl_fixed_to_int(surface_y);
            return;
        }
        int x = wl_fixed_to_int(surface_x);
        int y = wl_fixed_to_int(surface_y);
        sprops.yaw += (x - pointer_prev_x) * 0.3f;
        sprops.pitch += -(y - pointer_prev_y) * 0.3f;
        if (sprops.yaw > 180.)
            sprops.yaw -= 360.;
        if (sprops.yaw < -180.)
            sprops.yaw += 360.;
        if (sprops.pitch > 90.)
            sprops.pitch = 90.;
        if (sprops.pitch < -90.)
            sprops.pitch = -90.;
        pointer_prev_x = x;
        pointer_prev_y = y;
    }
}
void on_pointer_button(void *data, struct wl_pointer *wl_pointer,
                       uint32_t serial, uint32_t time, uint32_t button,
                       uint32_t state) {
    pointer_button_pressed = !!state;
    if (!state) {
        pointer_prev_x = -1;
        pointer_prev_y = -1;
    }
}

// ---- SHM buffer creation ----
static int create_shm_file(size_t size) {
    int fd = memfd_create("todo-this-name", 0);
    if (fd < 0)
        return -1;
    if (ftruncate(fd, size) < 0) {
        close(fd);
        return -1;
    }
    return fd;
}

static struct wl_buffer *create_buffer(struct client_state *state) {
    size_t stride = WIDTH * 4;
    size_t size = stride * HEIGHT;

    int fd = create_shm_file(size);
    if (fd < 0)
        return NULL;

    void *data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED) {
        close(fd);
        return NULL;
    }

    struct wl_shm_pool *pool = wl_shm_create_pool(state->shm, fd, size);
    struct wl_buffer *buffer = wl_shm_pool_create_buffer(
        pool, 0, WIDTH, HEIGHT, stride, WL_SHM_FORMAT_XRGB8888);

    wl_shm_pool_destroy(pool);
    close(fd);

    state->shm_buffer = data;

    // Fill buffer with black (0x000000)
    memset(data, 0, size);

    return buffer;
}

// ---- xdg_wm_base listener ----
static void wm_base_ping(void *data, struct xdg_wm_base *wm, uint32_t serial) {
    xdg_wm_base_pong(wm, serial);
}

static const struct xdg_wm_base_listener wm_base_listener = {.ping =
                                                                 wm_base_ping};

// ---- xdg_surface listener ----
static void xdg_surface_configure(void *data, struct xdg_surface *surface,
                                  uint32_t serial) {
    struct client_state *state = data;
    xdg_surface_ack_configure(surface, serial);
    state->configured = true;

    if (!state->buffer) {
        state->buffer = create_buffer(state);
        wl_surface_attach(state->surface, state->buffer, 0, 0);

        state->frame_callback = wl_surface_frame(state->surface);

        static const struct wl_callback_listener listener = {.done =
                                                                 frame_done};
        wl_callback_add_listener(state->frame_callback, &listener, state);
        wl_surface_commit(state->surface);
    }
}

static const struct xdg_surface_listener xdg_surface_listener = {
    .configure = xdg_surface_configure};

// ---- Registry listener ----
static void registry_add(void *data, struct wl_registry *registry,
                         uint32_t name, const char *interface,
                         uint32_t version) {
    struct client_state *state = data;

    if (strcmp(interface, wl_compositor_interface.name) == 0) {
        state->compositor =
            wl_registry_bind(registry, name, &wl_compositor_interface, 4);
    } else if (strcmp(interface, wl_shm_interface.name) == 0) {
        state->shm = wl_registry_bind(registry, name, &wl_shm_interface, 1);
    } else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
        state->wm_base =
            wl_registry_bind(registry, name, &xdg_wm_base_interface, 1);
        xdg_wm_base_add_listener(state->wm_base, &wm_base_listener, state);
    } else if (strcmp(interface, wl_seat_interface.name) == 0) {
        state->wl_seat =
            wl_registry_bind(registry, name, &wl_seat_interface, 1);
    }
}

static void registry_remove(void *data, struct wl_registry *registry,
                            uint32_t name) {}

static const struct wl_registry_listener registry_listener = {
    .global = registry_add, .global_remove = registry_remove};

// ---- Main ----
int main() {
    // render
    work_buffer = malloc(WIDTH * HEIGHT * 4);
    z_buffer = malloc(WIDTH * HEIGHT * sizeof(float));
    z_buffer_INF = malloc(WIDTH * HEIGHT * sizeof(float));
    for (int i = 0; i < WIDTH * HEIGHT; ++i) {
        z_buffer_INF[i] = INFINITY;
    }
    for (int i = 0; i < 6; ++i) {
        gravel_block_faces.face[i] = texture_gravel;
    }
    WORLD = VisibleWorld_Init();
    for (int i = 0; i < CHUNKS_COUNT; ++i) {
        struct R_ChunkInstance *c = CHUNKS + i;
        struct ChunkBase *c_base = malloc(sizeof(*c_base));
        c_base->pos = (ivec3_t){i, 0, 0};
        c_base->data = malloc(sizeof(*c_base->data));
        for (int x = 0; x < 16; ++x) {
            for (int y = 0; y < 16; ++y) {
                for (int z = 0; z < 16; ++z) {
                    if (y >= (1+i) && y <= (3+i) && z > 3 && z < 6) {
                        c_base->data->blocks[x][y][z] = gravel_block;
                    } else {
                        c_base->data->blocks[x][y][z].block_type =
                            BLOCKTYPE_AIR;
                        c_base->data->blocks[x][y][z].faces = 0;
                    }
                }
            }
        }
        c->base = c_base;
        c->mesh = malloc(sizeof(*c->mesh));
        c->mesh->dirty = false;
        c->mesh->polys = R_PolyVec_Init();
        VisibleWorld_PushChunk(WORLD, c_base);
    }
    for (int i = 0; i < CHUNKS_COUNT; ++i) {
        R_ChunkInstance_CalculateMesh(CHUNKS + i, WORLD);
    }
    // ---

    struct client_state state = {0};

    state.display = wl_display_connect(NULL);
    if (!state.display) {
        fprintf(stderr, "Failed to connect to Wayland\n");
        return 1;
    }

    state.registry = wl_display_get_registry(state.display);
    wl_registry_add_listener(state.registry, &registry_listener, &state);
    wl_display_roundtrip(state.display);

    state.surface = wl_compositor_create_surface(state.compositor);

    state.xdg_surface =
        xdg_wm_base_get_xdg_surface(state.wm_base, state.surface);
    xdg_surface_add_listener(state.xdg_surface, &xdg_surface_listener, &state);

    state.xdg_toplevel = xdg_surface_get_toplevel(state.xdg_surface);
    xdg_toplevel_set_app_id(state.xdg_toplevel, "org.ceheki.hlcraft");
    xdg_toplevel_set_title(state.xdg_toplevel, "HL-Craft Indev");

    wl_surface_commit(state.surface);

    struct WL_InputHelper ih;
    WL_inputhelper_init(&ih, state.wl_seat);

    ih.data = &state;
    ih.keyboard_keysym = on_keyboard_input;
    ih.pointer_motion = on_pointer_motion;
    ih.pointer_button = on_pointer_button;

    // Event loop
    state.render_since = 0;

    while (wl_display_dispatch(state.display) != -1) {
        // noop
    }

    wl_display_disconnect(state.display);
    return 0;
}
