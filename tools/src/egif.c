#include "egif.h"

#include <stdlib.h>

EGIF_FILE* egif_prep_enc(const uint32_t width, const uint32_t height,
                         const uint8_t bg_color_id,
                         const uint8_t* palette, const uint8_t palette_count)
{
    EGIF_FILE* egif = calloc(1, sizeof(EGIF_FILE));
    
    egif->magic = EGIF_MAGIC;
    egif->width = width;
    egif->height = height;
    egif->palette_count = palette_count;
    
    egif_set_bg_color(egif, bg_color_id);
    egif_set_palette(egif, palette, palette_count);
    
    egif->surface_buffer = (uint8_t*)malloc(width*height);
    egif_clear_surface_buffer(egif);

    return egif;
}

EGIF_FILE* egif_destroy(EGIF_FILE* egif)
{
    if(egif->surface_buffer)
    {
        free(egif->surface_buffer);
        egif->surface_buffer = NULL;
    }
    
    free(egif);
    
    return NULL;
}

void egif_set_palette(EGIF_FILE* const egif,
                      const uint8_t* palette, const uint8_t palette_count)
{
    uint16_t idx = 0;
    egif->palette_count = palette_count;
    
    for(uint16_t i = 0; i != palette_count; ++i)
    {
        egif->palette[i] = rgb_to_565_comp(palette[idx++],
                                           palette[idx++],
                                           palette[idx++]);
    }
}

void egif_process_surface(EGIF_FILE* const egif, const uint8_t* pixel_data,
                          const uint32_t surf_x, const uint32_t surf_y,
                          const uint32_t surf_w, const uint32_t surf_h)
{
    egif_clear_surface_buffer(egif);
    egif_copy_to_buffer(egif, pixel_data, surf_x, surf_y, surf_w, surf_h);
}

void egif_copy_to_buffer(EGIF_FILE* const egif, const uint8_t* pixel_data,
                         const uint32_t surf_x, const uint32_t surf_y,
                         const uint32_t surf_w, const uint32_t surf_h)
{
    for(uint32_t y = 0; y != surf_h; ++y)
    {
        uint8_t* target = &egif->surface_buffer[y*egif->width];
        const uint8_t* source = &pixel_data[((surf_y+y)*egif->width)+surf_x];
        memcpy(target, source, surf_w);
    }
}
