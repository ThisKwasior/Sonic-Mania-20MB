#pragma once

#define RSDK_MDL_MAGIC      (uint32_t)(4998221)

typedef struct
{
    uint8_t use_normals : 1;
    uint8_t use_textures : 1;
    uint8_t use_colours : 1;
} MDL_FLAGS;

typedef struct
{
    uint32_t magic;
    MDL_FLAGS flags;
    uint8_t face_vert_count;
    uint16_t vert_count;
    uint16_t frame_count;
} RSDK5_MDL_HEADER;

typedef struct
{
    float x;
    float y;
    float z;

    float nx;
    float ny;
    float nz;
} RSDK5_MDL_VERT;

static inline RSDK5_MDL_HEADER rsdk_mdl_read_header(FILE* f)
{
    RSDK5_MDL_HEADER mdl;
    fread(&mdl.magic, 4, 1, f);

    if(mdl.magic == RSDK_MDL_MAGIC)
    {
        fread(&mdl.flags, 1, 1, f);
        fread(&mdl.face_vert_count, 1, 1, f);
        fread(&mdl.vert_count, 2, 1, f);
        fread(&mdl.frame_count, 2, 1, f);
    }
    
    return mdl;
}