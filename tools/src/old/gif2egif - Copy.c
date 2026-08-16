#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "gif.h"
#include "egif.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

void gif_to_egif(const char* input, const char* output);
void egif_to_png(const char* input, const char* output);

int main(int argc, char** argv)
{
    if(argc < 3) return 0;
    
    uint32_t test;
    FILE* ftest = fopen(argv[1], "rb");
    fread(&test, 4, 1, ftest);
    fclose(ftest);
    
    switch(test)
    {
        case GIF_MAGIC:
            gif_to_egif(argv[1], argv[2]);
            break;
        case EGIF_MAGIC:
            egif_to_png(argv[1], argv[2]);
            break;
    }
    
    return 0;
}

void gif_to_egif(const char* input, const char* output)
{
    GIF_FILE gif = gif_read_file(input);
    
    EGIF_FILE* egif = egif_convert_from_data(&gif.palette[0], gif.palette_size,
                                             gif.head.width, gif.head.height,
                                             gif.image_data);
    
    egif_save_to_file(egif, output);
    
    egif = egif_free(egif);
    gif_free(&gif);
}

void egif_to_png(const char* input, const char* output)
{
    EGIF_FILE* egif = egif_load_from_file(input);
    uint8_t* data = egif_to_raw_rgb888(egif);
    stbi_write_png(output, egif->width, egif->height, 3, data, egif->width*3);
    free(egif);
}