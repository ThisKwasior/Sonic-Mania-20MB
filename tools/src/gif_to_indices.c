#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "gif.h"
#include "egif.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

/* https://www.willus.com/mingw/_binary.shtml */
#ifdef __MINGW32__
#include <fcntl.h>
#endif

int main(int argc, char** argv)
{
    if(argc == 1)
    {
        printf("Takes a GIF file as input and outputs its indices to stdout.\n");
        return 0;
    }
    
#ifdef __MINGW32__
    _setmode(_fileno(stdout), _O_BINARY);
#endif

    GIF_FILE gif = gif_read_file(argv[1]);
    const uint32_t buffer_size = gif.head.width*gif.head.height;
    int len = 0;
    unsigned char* png = stbi_write_png_to_mem((const unsigned char*)gif.image_data,
                                               gif.head.width, gif.head.width,
                                               gif.head.height, 1, &len);
    fwrite(png, 1, len, stdout);
    free(png);

    return 0;
}