#include "input-helper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <wayland-client-protocol.h>

// wl_pointer -related {{{

static void pointer_listener_enter(void *data, struct wl_pointer *wl_pointer,
                                   uint32_t serial, struct wl_surface *surface,
                                   wl_fixed_t surface_x, wl_fixed_t surface_y) {
    struct WL_InputHelper *ih = data;
    if (ih->pointer_enter) {
        ih->pointer_enter(ih->data, wl_pointer, serial, surface, surface_x,
                          surface_y);
    }
}
static void pointer_listener_leave(void *data, struct wl_pointer *wl_pointer,
                                   uint32_t serial,
                                   struct wl_surface *surface) {}
static void pointer_listener_frame(void *data, struct wl_pointer *wl_pointer) {
    struct WL_InputHelper *ih = data;
    if (ih->pointer_frame) {
        ih->pointer_frame(ih->data, wl_pointer);
    }
}

static void pointer_listener_motion(void *data, struct wl_pointer *wl_pointer,
                                    uint32_t time, wl_fixed_t surface_x,
                                    wl_fixed_t surface_y) {
    struct WL_InputHelper *ih = data;
    if (ih->pointer_motion) {
        ih->pointer_motion(ih->data, wl_pointer, time, surface_x, surface_y);
    }
}
static void pointer_listener_button(void *data, struct wl_pointer *wl_pointer,
                                    uint32_t serial, uint32_t time,
                                    uint32_t button, uint32_t state) {
    struct WL_InputHelper *ih = data;
    if (ih->pointer_button) {
        ih->pointer_button(ih->data, wl_pointer, serial, time, button, state);
    }
}

/* pointer_listener_* shit {{{ */
static void pointer_listener_axis(void *data, struct wl_pointer *pointer,
                                  uint32_t time, uint32_t axis,
                                  wl_fixed_t value) {}

static void pointer_listener_axis_source(void *data, struct wl_pointer *pointer,
                                         uint32_t axis_source) {}

static void pointer_listener_axis_stop(void *data, struct wl_pointer *pointer,
                                       uint32_t time, uint32_t axis) {}

static void pointer_listener_axis_discrete(void *data,
                                           struct wl_pointer *pointer,
                                           uint32_t axis, int32_t discrete) {}

static void pointer_listener_axis_value120(void *data,
                                           struct wl_pointer *pointer,
                                           uint32_t axis, int32_t value120) {}

static void pointer_listener_axis_relative_direction(void *data,
                                                     struct wl_pointer *pointer,
                                                     uint32_t axis,
                                                     uint32_t direction) {}

/* }}} */

static struct wl_pointer_listener pointer_listener = {
    .enter = pointer_listener_enter,
    .leave = pointer_listener_leave,
    .motion = pointer_listener_motion,
    .button = pointer_listener_button,
    .frame = pointer_listener_frame,
    .axis = pointer_listener_axis,
    .axis_source = pointer_listener_axis_source,
    .axis_stop = pointer_listener_axis_stop,
    .axis_discrete = pointer_listener_axis_discrete,
    .axis_value120 = pointer_listener_axis_value120,
    .axis_relative_direction = pointer_listener_axis_relative_direction,
};

// }}}

// wl_keyboard -related {{{

static void keyboard_listener_enter(void *data, struct wl_keyboard *wl_keyboard,
                                    uint32_t serial, struct wl_surface *surface,
                                    struct wl_array *keys) {}
static void keyboard_listener_leave(void *data, struct wl_keyboard *wl_keyboard,
                                    uint32_t serial,
                                    struct wl_surface *surface) {}
static void keyboard_listener_modifiers(void *data,
                                        struct wl_keyboard *wl_keyboard,
                                        uint32_t serial,
                                        uint32_t mods_depressed,
                                        uint32_t mods_latched,
                                        uint32_t mods_locked, uint32_t group) {
    struct WL_InputHelper *ih = data;
    xkb_state_update_mask(ih->xkb_state, mods_depressed, mods_latched,
                          mods_locked, 0, 0, group);
}
static void keyboard_listener_repeat_info(void *data,
                                          struct wl_keyboard *wl_keyboard,
                                          int32_t rate, int32_t delay) {}

static void keyboard_listener_keymap(void *data,
                                     struct wl_keyboard *wl_keyboard,
                                     uint32_t format, int32_t fd,
                                     uint32_t size) {
    struct WL_InputHelper *ih = data;

    if (format != WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1) {
        fprintf(stderr, "wl_keyboard: unsupported keymap\n");
        abort();
    }

    char *map_str = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    ih->xkb_context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);

    ih->xkb_keymap = xkb_keymap_new_from_string(ih->xkb_context, map_str,
                                                XKB_KEYMAP_FORMAT_TEXT_V1,
                                                XKB_KEYMAP_COMPILE_NO_FLAGS);

    munmap(map_str, size);
    close(fd);

    ih->xkb_state = xkb_state_new(ih->xkb_keymap);
}

static void keyboard_listener_key(void *data, struct wl_keyboard *wl_keyboard,
                                  uint32_t serial, uint32_t time, uint32_t key,
                                  uint32_t state) {
    struct WL_InputHelper *ih = data;
    if (ih->keyboard_keysym) {
        xkb_keysym_t sym = xkb_state_key_get_one_sym(ih->xkb_state, key + 8);
        ih->keyboard_keysym(ih->data, wl_keyboard, serial, time, sym, state);
    }
}

static struct wl_keyboard_listener kb_listener = {
    .enter = keyboard_listener_enter,
    .leave = keyboard_listener_leave,
    .repeat_info = keyboard_listener_repeat_info,
    .keymap = keyboard_listener_keymap,
    .key = keyboard_listener_key,
    .modifiers = keyboard_listener_modifiers,
};

// }}}

void WL_inputhelper_init(struct WL_InputHelper *input_helper,
                         struct wl_seat *wl_seat) {
    memset(input_helper, 0, sizeof(*input_helper));

    input_helper->wl_keyboard = wl_seat_get_keyboard(wl_seat);
    wl_keyboard_add_listener(input_helper->wl_keyboard, &kb_listener,
                             input_helper);

    input_helper->wl_pointer = wl_seat_get_pointer(wl_seat);
    wl_pointer_add_listener(input_helper->wl_pointer, &pointer_listener,
                            input_helper);
}

void WL_inputhelper_clear(struct WL_InputHelper *input_helper) {
    input_helper->keyboard_keysym = 0;
    input_helper->pointer_enter = 0;
    input_helper->pointer_motion = 0;
    input_helper->pointer_button = 0;
    input_helper->pointer_frame = 0;
}

/* vim: set foldmethod=marker : */
