#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "gif.h"
#include "rgb.h"
#include "vrle.h"
#include "miniz.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

const uint32_t EGIF_MAGIC = 1179207493;

void compress_clut_vrle(uint8_t* palette, FILE* fout);
uint64_t get_file_size(FILE* f);

int main(int argc, char** argv)
{
    if(argc == 1) return 0;
    
    GIF_FILE gif = gif_read_file(argv[1]);
    
    //stbi_write_png("./gif2egif_temp_palette.png", 16, 16, 3, &gif.palette, 16*3);
    //stbi_write_png("output.png", gif.head.width, gif.head.height, 1, gif.image_data, gif.head.width);
    
    const uint32_t fin_size = gif.head.width*gif.head.height;
    //uint8_t* fin_data = &gif.image_data[0];
    
    mz_ulong fout_size = fin_size*2;
    uint8_t* fout_data = calloc(fin_size*2, 1);
    
    mz_compress2(fout_data, &fout_size, gif.image_data, fin_size, MZ_UBER_COMPRESSION);
    const uint32_t fout_size_u32 = fout_size;
    
    FILE* fout = NULL;
    if(argc == 3)
        fout = fopen(argv[2], "wb");
    else
        fout = fopen("output.bin", "wb");
        
    fwrite(&EGIF_MAGIC, 4, 1, fout);
    fwrite(&gif.head.width, 2, 1, fout);
    fwrite(&gif.head.height, 2, 1, fout);
    
    compress_clut_vrle(&gif.palette[0], fout);

    fprintf(fout, "CMPZ");
    fwrite(&fin_size, 4, 1, fout);
    fwrite(&fout_size_u32, 4, 1, fout);
    fwrite(fout_data, fout_size_u32, 1, fout);

    fclose(fout);

    gif_free(&gif);
    
    return 0;
}

void compress_clut_vrle(uint8_t* palette, FILE* fout)
{
    uint16_t* clut_5551 = (uint16_t*)calloc(2, 256);
    uint16_t clut_rem = 256;
    
    /* Convert RGBA to RGBA5551 */
    for(uint16_t i = 0; i != 256; ++i)
    {
        RGBA8888 col;
        col.r = palette[i*3];
        col.g = palette[i*3+1];
        col.b = palette[i*3+2];
        RGBA5551 packed = rgba_to_5551(col);
        clut_5551[i] = *(uint16_t*)&packed;
    }
    
    /* VRLE the data */
    while(clut_rem)
    {
        uint16_t* cur_color_ptr = &clut_5551[256-clut_rem];
        VRLE_HEADER head = vrle_count_packets(VRLE_TYPE_U16, (uint8_t*)cur_color_ptr, clut_rem*2);
        
        fwrite(&head, 1, 1, fout);
        fwrite(cur_color_ptr, 2, 1, fout);
        
        clut_rem -= head.len;
    }
    
    free(clut_5551);
}

uint64_t get_file_size(FILE* f)
{
    const uint64_t posold = ftell(f);
    fseek(f, 0, SEEK_END);
    const uint64_t pos = ftell(f);
    fseek(f, posold, SEEK_SET);
    return pos;
}