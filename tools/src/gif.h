#pragma once

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>

#include "rgb.h"

#define STBI_NO_JPEG
#define STBI_NO_BMP
#define STBI_NO_PSD
#define STBI_NO_TGA
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_PNM 
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define GIF_MAGIC   (uint32_t)(944130375)

typedef struct
{
    uint8_t size : 3;       // physical size = 2^(flags.size + 1)
    uint8_t sort : 1;       // Indicates if the table is sorted by importance
    uint8_t colorRes : 3;   // Indicates the richness of the original pallet
    uint8_t enabled : 1;
} GIF_GCT_FLAGS;

typedef struct
{
    char magic[3];      /* GIF */
    char version[3];
    uint16_t width;
    uint16_t height;
    GIF_GCT_FLAGS flags;
    uint8_t bg_color_id;
    uint8_t pixel_aspect;
} GIF_HEADER;

typedef struct
{
    GIF_HEADER head;
    uint16_t palette_size;
    uint8_t palette[256*3];
    uint8_t* image_data;
} GIF_FILE;

static inline uint8_t gif_rgb_to_index(RGB888 col, GIF_FILE* gif)
{
    for(int i = 0; i != gif->palette_size; ++i)
    {
        RGB888 cur_pc = *(RGB888*)&gif->palette[i*3];
        if((col.r==cur_pc.r)&&(col.g==cur_pc.g)&&(col.b==cur_pc.b))
        {
            return i;
        }
    }
    
    return gif->head.bg_color_id;
}

static inline GIF_FILE gif_read_file(const char* filename)
{
    FILE* f = fopen(filename, "rb");
    GIF_FILE gif = {0};
    
    if(f)
    {
        fread(&gif.head.magic[0], 1, 3, f);
        
        if(gif.head.magic[0] == 'G'
           && gif.head.magic[1] == 'I'
           && gif.head.magic[2] == 'F')
        {
            fread(&gif.head.version, 3, 1, f);
            fread(&gif.head.width, 2, 1, f);
            fread(&gif.head.height, 2, 1, f);
            fread(&gif.head.flags, 1, 1, f);
            fread(&gif.head.bg_color_id, 1, 1, f);
            fread(&gif.head.pixel_aspect, 1, 1, f);

            gif.palette_size = pow(2.f, gif.head.flags.size+1);
            fread(&gif.palette[0], gif.palette_size, 3, f);
            
            gif.image_data = (uint8_t*)calloc(1, gif.head.width*gif.head.height);
        }
        
        fclose(f);
        
        int x,y,n;
        unsigned char* data = stbi_load(filename, &x, &y, &n, 4);
        
        for(int i = 0; i != (gif.head.width*gif.head.height); ++i)
        {
            RGB888 cur_pixel = *(RGB888*)&data[i*4];
            gif.image_data[i] = gif_rgb_to_index(cur_pixel, &gif);
        }
        
        free(data);
    }
    
    return gif;
}

static inline void gif_free(GIF_FILE* gif)
{
    free(gif->image_data);
    gif->image_data = NULL;
}