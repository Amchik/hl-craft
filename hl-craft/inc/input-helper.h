/* input-helper.h
 * Helper structure with keyboard/pointer input from wayland server.
 * Originally written for prtsc-plus.
 */

#pragma once

#include <wayland-client-protocol.h>
#include <xkbcommon/xkbcommon.h>

struct WL_InputHelper {
    struct wl_keyboard *wl_keyboard;
    struct wl_pointer *wl_pointer;

    struct xkb_context *xkb_context;
    struct xkb_keymap *xkb_keymap;
    struct xkb_state *xkb_state;

    /* Shared data to pass in all handlers
     */
    void *data;

    /* wl_pointer events. Same as original events.
     */
    void (*pointer_enter)(void *data, struct wl_pointer *wl_pointer,
                          uint32_t serial, struct wl_surface *surface,
                          wl_fixed_t surface_x, wl_fixed_t surface_y);
    void (*pointer_motion)(void *data, struct wl_pointer *wl_pointer,
                           uint32_t time, wl_fixed_t surface_x,
                           wl_fixed_t surface_y);
    void (*pointer_button)(void *data, struct wl_pointer *wl_pointer,
                           uint32_t serial, uint32_t time, uint32_t button,
                           uint32_t state);
    void (*pointer_frame)(void *data, struct wl_pointer *wl_pointer);

    /* Custom wl_keyboard event. Like `wl_keyboard::key` but with keysym instead
     * of key.
     */
    void (*keyboard_keysym)(void *data, struct wl_keyboard *wl_keyboard,
                            uint32_t serial, uint32_t time, xkb_keysym_t key,
                            uint32_t state);
};

/* Initialize input helper.
 * Note that all fields will be overwritten with zeroes, so it can cause
 * `wl_keyboard` and `wl_pointer` to leak.
 */
void WL_inputhelper_init(struct WL_InputHelper *input_helper,
                         struct wl_seat *wl_seat);

/* Clear all callbacks.
 * */
void WL_inputhelper_clear(struct WL_InputHelper *input_helper);
