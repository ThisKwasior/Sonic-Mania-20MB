#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "gif.h"
#include "egif.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define PEP_IMPLEMENTATION
#include "pep.h"

int main(int argc, char** argv)
{
    if(argc == 1) return 0;
    
    GIF_FILE gif = gif_read_file(argv[1]);
    const uint32_t data_size = gif.head.width*gif.head.height;
    
    uint32_t* pixel_data = calloc(data_size, 4);
    
    for(uint32_t i = 0; i != data_size; ++i)
    {
        pixel_data[i] = gif.image_data[i];
    }
    
    pep peepee = pep_compress(pixel_data, gif.head.width, gif.head.height, pep_rgba, pep_8bit);
    pep_save(&peepee, "shit.pep");
    pep peepee2 = pep_load("shit.pep");
    
    return 0;
}