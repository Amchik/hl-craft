#pragma once

#include <stdint.h>

struct R_Texture16x16 {
    uint32_t *texture;
};

enum R_TextureTriagPlace {
    R_TTP_FAR_HOR,
    R_TTP_NEAR_HOR,
    R_TTP_FAR_VERT,
    R_TTP_NEAR_VERT,
};

enum R_Face {
    R_FACE_TOP = 0,
    R_FACE_BOTTOM = 1,
    // positive/negative x/z
    R_FACE_XP = 2,
    R_FACE_ZP = 3,
    R_FACE_XN = 4,
    R_FACE_ZN = 5,
};

struct R_BlockFaces {
    struct R_Texture16x16 face[6];
};
