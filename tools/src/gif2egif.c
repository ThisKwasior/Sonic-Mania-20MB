#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "gif.h"
#include "egif.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int main(int argc, char** argv)
{
    if(argc == 1) return 0;
    
    GIF_FILE gif = gif_read_file(argv[1]);
    
    EGIF_FILE* egif = egif_prep_enc(gif.head.width, gif.head.height,
                                    gif.head.bg_color_id,
                                    gif.palette, gif.palette_size);
    
    egif_process_surface(egif, gif.image_data, 39, 48, 223, 186);
    
    for(uint32_t i = 0; i != gif.head.width*gif.head.height; ++i)
        printf("%02x", egif->surface_buffer[i]);
    
    return 0;
}