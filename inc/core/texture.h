#pragma once

#include <stdint.h>

// Little-endian XRGB8888 color (see WL_SHM_FORMAT_XRGB8888)
typedef uint32_t clr_xrgb8_t;

struct Texture16x16 {
    const clr_xrgb8_t *texture;
};

struct AbstractTextureRef {
    union {
        struct Texture16x16 _ty_16x16;
    } _texture;
    uint8_t _texture_ty;
};

enum AbstractTextureRefType {
    TEXTURE_TYPE_16X16 = 1,
};

static inline clr_xrgb8_t
texture_pixel(const struct AbstractTextureRef *texture, float u, float v) {
    switch (texture->_texture_ty) {
    case TEXTURE_TYPE_16X16:
        return texture->_texture._ty_16x16
            .texture[(int)(u * 16) % 16 * 16 + (int)(v * 16) % 16];
    default:
        // safety moment
        __builtin_unreachable();
    }
}
