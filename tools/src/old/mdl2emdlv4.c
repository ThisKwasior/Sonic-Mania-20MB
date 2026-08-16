#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "rsdk_mdl.h"
#include "emdl.h"
#include "bit_pack.h"
#include "rgb.h"
#include "fixed.h"
#include "vrle.h"

uint64_t get_file_size(FILE* f);

void compress_clut_vrle(FILE* fin, const uint16_t vert_count, FILE* fout);

void get_scale_factors(FILE* fin, RSDK5_MDL_HEADER mdl, EMDL_FLAGS emdl, float* x, float* y, float* z);

int main(int argc, char** argv)
{
    if(argc == 1) return 0;
    
    FILE* fmdl = fopen(argv[1], "rb");
    const uint64_t fmdl_size = get_file_size(fmdl);
    
    RSDK5_MDL_HEADER mdl = rsdk_mdl_read_header(fmdl);
    
    if(mdl.magic == RSDK_MDL_MAGIC)
    {
        // open for writing
        FILE* femdl = NULL;
        
        if(argc == 3)
            femdl = fopen(argv[2], "wb");
        else
            femdl = fopen("output.bin", "wb");
        
        const uint32_t emdl_magic = RSDK_EMDL_MAGIC;
        EMDL_FLAGS emdl_flags = {0};
        emdl_flags.use_normals = mdl.flags.use_normals;
        emdl_flags.use_textures = mdl.flags.use_textures;
        emdl_flags.use_colours = mdl.flags.use_colours;
        //emdl_flags.indices_u8 = mdl.vert_count < 256 ? 1 : 0;
        emdl_flags.indices_u8 = 0;
        
        fwrite(&emdl_magic, 4, 1, femdl);
        fwrite(&emdl_flags, 1, 1, femdl);
        fwrite(&mdl.face_vert_count, 1, 1, femdl);
        fwrite(&mdl.vert_count, 2, 1, femdl);
        fwrite(&mdl.frame_count, 2, 1, femdl);
        
        /* Colors */
        if(mdl.flags.use_colours)
        {
            compress_clut_vrle(fmdl, mdl.vert_count, femdl);
        }
        
        /* Indices */
        uint16_t indices_count = 0;
        fread(&indices_count, 2, 1, fmdl);
        fwrite(&indices_count, 2, 1, femdl);
        
        for(uint16_t i = 0; i != indices_count; ++i)
        {
            uint16_t index = 0;
            fread(&index, 2, 1, fmdl);
            uint8_t index_u8 = index;
            
            if(emdl_flags.indices_u8)
                fwrite(&index_u8, 1, 1, femdl);
            else
                fwrite(&index, 2, 1, femdl);
        }
        
        /* Vertices */
        int64_t fpos_vert = ftell(fmdl);
        float scale_facs[3] = {0};
        get_scale_factors(fmdl, mdl, emdl_flags, &scale_facs[0], &scale_facs[1], &scale_facs[2]);
        fseek(fmdl, fpos_vert, SEEK_SET);
        
        fwrite(&scale_facs, sizeof(float), 3, femdl);
        
        uint32_t fvmax = mdl.frame_count*mdl.vert_count;
        
        for(uint16_t i = 0; i != fvmax; ++i)
        {
            float pos[3] = {0};
            float nrm[3] = {0};
            
            fread(&pos[0], 4, 3, fmdl);
            PACK_VEC3_NRM_S24 pos_fp = f32_to_nrm_vec24(pos[0]/scale_facs[0],
                                                        pos[1]/scale_facs[1],
                                                        pos[2]/scale_facs[2]);
            fwrite(&pos_fp, sizeof(PACK_VEC3_NRM_S24), 1, femdl);

            if(emdl_flags.use_normals)
            {
                fread(&nrm[0], 4, 3, fmdl);
                PACK_VEC3_S16 nrmp = f32_to_packed_vec16(nrm[0], nrm[1], nrm[2]);
                fwrite(&nrmp, sizeof(PACK_VEC3_S16), 1, femdl);
            }
        }
        
        fclose(femdl);
    }
    
    fclose(fmdl);
    return 0;
}

uint64_t get_file_size(FILE* f)
{
    const uint64_t posold = ftell(f);
    fseek(f, 0, SEEK_END);
    const uint64_t pos = ftell(f);
    fseek(f, posold, SEEK_SET);
    return pos;
}

void compress_clut_vrle(FILE* fin, const uint16_t vert_count, FILE* fout)
{
    uint16_t* clut_5551 = (uint16_t*)calloc(2, vert_count);
    uint16_t clut_rem = vert_count;
    
    /* Convert RGBA to RGBA5551 */
    for(uint16_t i = 0; i != vert_count; ++i)
    {
        RGBA8888 rgba;
        fread(&rgba.b, 1, 1, fin);
        fread(&rgba.g, 1, 1, fin);
        fread(&rgba.r, 1, 1, fin);
        fread(&rgba.a, 1, 1, fin);
        RGBA5551 packed = rgba_to_5551(rgba);
        clut_5551[i] = *(uint16_t*)&packed;
    }
    
    /* VRLE the data */
    while(clut_rem)
    {
        uint16_t* cur_color_ptr = &clut_5551[vert_count-clut_rem];
        VRLE_HEADER head = vrle_count_packets(VRLE_TYPE_U16, (uint8_t*)cur_color_ptr, clut_rem*2);
        
        fwrite(&head, 1, 1, fout);
        fwrite(cur_color_ptr, 2, 1, fout);
        
        clut_rem -= head.len;
    }
    
    free(clut_5551);
}

void get_scale_factors(FILE* fin, RSDK5_MDL_HEADER mdl, EMDL_FLAGS emdl, float* x, float* y, float* z)
{
    (*x) = (*y) = (*z) = 0.f;
    
    uint32_t fvmax = mdl.frame_count*mdl.vert_count;
    
    for(uint16_t i = 0; i != fvmax; ++i)
    {
        float pos[3] = {0};
        
        fread(&pos[0], 4, 3, fin);
        
        /* Poor man's fabs() */
        if(pos[0] < 0.f) pos[0] *= (-1.f);
        if(pos[1] < 0.f) pos[1] *= (-1.f);
        if(pos[2] < 0.f) pos[2] *= (-1.f);
        
        /* Poor man's max() */
        if((*x) < pos[0]) (*x) = pos[0];
        if((*y) < pos[1]) (*y) = pos[1];
        if((*z) < pos[2]) (*z) = pos[2];

        if(emdl.use_normals)
        {
            fseek(fin, 12, SEEK_CUR);
        }
    }
}