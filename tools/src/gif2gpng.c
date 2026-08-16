#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define KWASLIB_LITTLE_ENDIAN
#include "kwaslib/endianness.h"

#include "gif.h"

/* https://www.willus.com/mingw/_binary.shtml */
#ifdef __MINGW32__
#include <fcntl.h>
#endif

/*
    Returns true if IEND was reached
*/
uint8_t png_read_packet(FILE* in, FILE* out, uint32_t* size_accum);

int main(int argc, char** argv)
{
    if(argc == 1)
    {
        printf("Takes a GIF file and a PNG with indices (via stdin)\n");
        printf("and outputs a file to stdout.\n");
        return 0;
    }
    
#ifdef __MINGW32__
    _setmode(_fileno(stdin), _O_BINARY);
    _setmode(_fileno(stdout), _O_BINARY);
#endif
    
    GIF_FILE gif = gif_read_file(argv[1]);
    
    const uint32_t gpng_magic = 1196314695;

    /* GPNG Header */
    fwrite(&gpng_magic, 4, 1, stdout);
    fwrite(&gif.head.width, 2, 1, stdout);
    fwrite(&gif.head.height, 2, 1, stdout);
    fwrite(&gif.palette_size, 2, 1, stdout);
    
    /* RGB565 palette */
    for(uint16_t i = 0; i != gif.palette_size; ++i)
    {
        const uint32_t col_id = i*3; 
        RGB565 col = rgb_to_565_comp(gif.palette[col_id],
                                     gif.palette[col_id+1],
                                     gif.palette[col_id+2]);
                                     
        fwrite(&col, sizeof(RGB565), 1, stdout);
    }

    /* PNG-encoded indices. Preferably processed through oxipng */

    uint32_t u32buf = 0;
    
    fread(&u32buf, 4, 1, stdin);
    
    /* ‰PNG */
    if(u32buf == 1196314761)
    {
        fwrite(&u32buf, 4, 1, stdout);
        u32buf = 0;
        fread(&u32buf, 4, 1, stdin);
        fwrite(&u32buf, 4, 1, stdout);
        
        uint32_t png_size = 8;
        while(png_read_packet(stdin, stdout, &png_size) == 0){}
        
        //fwrite(&png_size, 4, 1, stdout);
    }

    return 0;
}

uint8_t png_read_packet(FILE* in, FILE* out, uint32_t* size_accum)
{
    uint8_t is_iend = 0;
    uint32_t len = 0;
    uint32_t crc = 0;
    char ident[4];
    
    fread(&len, 4, 1, in);
    fwrite(&len, 4, 1, out);
    len = ed_swap_endian_32(len);
    
    fread(&ident[0], 4, 1, in);
    
    if(strncmp(ident, "IEND", 4) == 0)
        is_iend = 1;

    fwrite(&ident[0], 4, 1, out);
    
    for(uint32_t i = 0; i != len; ++i)
    {
        uint8_t c = 0;
        fread(&c, 1, 1, in);
        fwrite(&c, 1, 1, out);
    }
    
    fread(&crc, 4, 1, in);
    fwrite(&crc, 4, 1, out);
    
    *size_accum += 4*3 + len;
    
    return is_iend;
}