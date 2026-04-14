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
    R_FACE_TOP,
    R_FACE_BOTTOM,
    // positive/negative x/z
    R_FACE_PP,
    R_FACE_PN,
    R_FACE_NP,
    R_FACE_NN,
};

struct R_BlockFaces {
    struct R_Texture16x16 face[6];
};
