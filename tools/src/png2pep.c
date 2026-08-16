#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define PEP_IMPLEMENTATION
#include "pep.h"

int main(int argc, char** argv)
{
    if(argc == 1) return 0;
    
    int x,y,n;
    unsigned char* data = stbi_load(argv[1], &x, &y, &n, 4);
    
    pep peepee = pep_compress((uint32_t*)&data[0], x, y, pep_rgba, pep_1bit);
    pep_save(&peepee, "shit.pep");
    pep peepee2 = pep_load("shit.pep");
    
    uint32_t* pixels = pep_decompress(&peepee2, pep_rgba, 0, 0);
    stbi_write_png("shit.pep.png", x, y, 4, pixels, x*4);
    
    return 0;
}