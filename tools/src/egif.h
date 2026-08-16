#pragma once

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "rgb.h"

#define EGIF_MAGIC              (uint32_t)(1179207493)

#define EGIF_PACKET_SMALL       (uint8_t)(1)
#define EGIF_PACKET_MEDIUM      (uint8_t)(2)
#define EGIF_PACKET_BIG         (uint8_t)(3)

#define EGIF_PACKET_SMALL_MAX   (uint32_t)(63)
#define EGIF_PACKET_MEDIUM_MAX  (uint32_t)(16383)
#define EGIF_PACKET_BIG_MAX     (uint32_t)(4194303)

#define EGIF_BLOCK_ORIENT_NRM   (uint8_t)(0)
#define EGIF_BLOCK_ORIENT_HOR   (uint8_t)(1)
#define EGIF_BLOCK_ORIENT_VER   (uint8_t)(2)
#define EGIF_BLOCK_ORIENT_HVR   (uint8_t)(3)

/*
    Structures
*/
typedef union
{
    struct
    {
        uint8_t orient : 2;
        uint8_t block_id : 6;
    } s;
    
    struct
    {
        uint16_t orient : 2;
        uint16_t block_id : 14;
    } m;
    
    struct
    {
        uint32_t orient : 2;
        uint32_t block_id : 22;
    } b;
} EGIF_PACKET;

typedef struct
{
    uint8_t data[4][4*4];   /* Normal, hor-flip, ver-flip, hor-ver-flip */
    uint32_t hash[4];
} EGIF_BLOCK;

typedef struct
{
    uint32_t magic;
    uint16_t width;
    uint16_t height;
    uint32_t blocks_count;
    uint8_t packet_size;
    
    RGB565 palette[256];
    uint8_t palette_count;
    uint8_t bg_color;           /* Color that's gonna be used in extending non-4 blocks */

    uint8_t* surface_buffer;    /* For processing added surfaces. Freed with egif_finalize() */

} EGIF_FILE;

/*
    Implementation
*/

/*
    Prepares and returns the egif structure ready for encoding planes.
*/
EGIF_FILE* egif_prep_enc(const uint32_t width, const uint32_t height,
                         const uint8_t bg_color_id,
                         const uint8_t* palette, const uint8_t palette_count);

/*
    Frees all data in the EGIF.
    Returns NULL.
*/
EGIF_FILE* egif_destroy(EGIF_FILE* egif);

/*
    Palette array should be RGB24
*/
void egif_set_palette(EGIF_FILE* const egif,
                      const uint8_t* palette, const uint8_t palette_count);

/*

*/
void egif_process_surface(EGIF_FILE* const egif, const uint8_t* pixel_data,
                          const uint32_t surf_x, const uint32_t surf_y,
                          const uint32_t surf_w, const uint32_t surf_h);

/*
    Copies the cut-out surface to internal buffer at (0,0)
*/
void egif_copy_to_buffer(EGIF_FILE* const egif, const uint8_t* pixel_data,
                         const uint32_t surf_x, const uint32_t surf_y,
                         const uint32_t surf_w, const uint32_t surf_h);

/*
    Clears the internal surface buffer to bg color.
*/
static inline void egif_clear_surface_buffer(const EGIF_FILE* egif)
{
    memset(egif->surface_buffer, egif->bg_color, egif->width*egif->height);
}
                  
/*
    Color index that's gonna be used in extending non-full blocks.
*/
static inline void egif_set_bg_color(EGIF_FILE* const egif, const uint8_t index)
{
    egif->bg_color = index;
}