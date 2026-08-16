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

    char name_buf[64] = {0};
    uint32_t buffer_size = gif.head.width*gif.head.height;
    char* buffer = calloc(gif.head.width*gif.head.height, 1);

    sprintf(name_buf, "plane_x.png\0");
    stbi_write_png(name_buf, gif.head.width, gif.head.height, 1, gif.image_data, gif.head.width);

    for(uint8_t planeid = 0; planeid != 8; ++planeid)
    {
        for(uint32_t i = 0; i != buffer_size; ++i)
        {
            const uint8_t val = gif.image_data[i]&(1<<planeid);
            
            if(val) buffer[i] = 0xFF;
            else buffer[i] = 0;
        }
        
        sprintf(name_buf, "plane_%d.png\0", planeid);
        stbi_write_png(name_buf, gif.head.width, gif.head.height, 1, buffer, gif.head.width);
    }
    
    return 0;
}