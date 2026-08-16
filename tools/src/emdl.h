#pragma once

#include "bit_pack.h"

#define RSDK_EMDL_MAGIC     (uint32_t)(1279544645)

typedef struct
{
    uint8_t use_normals : 1;
    uint8_t use_textures : 1;
    uint8_t use_colours : 1;
    uint8_t indices_u8 : 1;
} EMDL_FLAGS;

typedef struct
{
    uint32_t magic;
    EMDL_FLAGS flags;
    uint8_t face_vert_count;
    uint16_t vert_count;
    uint16_t frame_count;
} EMDL_HEADER;

typedef struct
{
    PACK_VEC3_S32 pos;
    PACK_VEC3_S16 nrm;
} EMDL_VERTEX;

typedef struct
{
    PACK_VEC3_S16 pos;
    PACK_VEC3_S16 nrm;
} EMDL_VERTEX_QUANT;