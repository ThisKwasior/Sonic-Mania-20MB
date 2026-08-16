#pragma once

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "rgb.h"
#include "vrle.h"
#include "kwaslib/crc32.h"

#define EGIF_MAGIC              (uint32_t)(1179207493)

#define EGIF_PACKET_SMALL       (uint8_t)(0)
#define EGIF_PACKET_MEDIUM      (uint8_t)(1)
#define EGIF_PACKET_BIG         (uint8_t)(2)

#define EGIF_PACKET_SMALL_MAX   (uint32_t)(63)
#define EGIF_PACKET_MEDIUM_MAX  (uint32_t)(16383)
#define EGIF_PACKET_BIG_MAX     (uint32_t)(4194303)

#define EGIF_BLOCK_ORIENT_NRM   (uint8_t)(0)
#define EGIF_BLOCK_ORIENT_HOR   (uint8_t)(1)
#define EGIF_BLOCK_ORIENT_VER   (uint8_t)(2)
#define EGIF_BLOCK_ORIENT_HVR   (uint8_t)(3)

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
    uint8_t data[4*4];
    uint32_t hash;
} EGIF_BLOCK;

typedef struct
{
    EGIF_BLOCK blocks[4]; /* Normal, hor-flip, ver-flip, hor-ver-flip */
} EGIF_BLOCK_CODER;

typedef struct
{
    uint32_t magic;
    uint16_t width;
    uint16_t height;
    uint32_t blocks_count;
    uint8_t packet_size;
    uint16_t palette_count;
    uint32_t vrle_palette_size;
    uint32_t vrle_blocks_size;
    uint32_t vrle_packets_size;
    
    RGBA5551 palette[256];
    EGIF_BLOCK_CODER* blocks;
    EGIF_PACKET* packets;

} EGIF_FILE;

/*
    Converts RGBA888 palette to internal RGBA5551.
    Assumess palette data is 768 bytes long.
*/
static inline void egif_pal_rgba_to_5551(EGIF_FILE* egif, const uint8_t* palette)
{
    RGB888* palette_rgb888 = (RGB888*)palette;
    
    for(uint32_t i = 0; i != egif->palette_count; ++i)
    {
        egif->palette[i] = rgba_888_to_5551(palette_rgb888[i]);
    }
}

/*
    Returns a block
*/
static inline EGIF_BLOCK egif_get_block(EGIF_FILE* egif, const uint8_t* pixel_data,
                                        const uint32_t x, const uint32_t y)
{
    EGIF_BLOCK block = {0};

    for(uint32_t by = 0; by != 4; by++)
    {
        for(uint32_t bx = 0; bx != 4; bx++)
        {
            const uint32_t bidx = rgba_get_index(bx, by, 4, 1);
            const uint32_t pidx = rgba_get_index(x+bx, y+by, egif->width, 1);
            block.data[bidx] = pixel_data[pidx];
        }
    }

    block.hash = crc32_encode(&block.data[0], 16);

    return block;
}

/*
    Flips block horizontally
*/
static inline EGIF_BLOCK egif_block_flip_horizontal(EGIF_BLOCK block)
{
    EGIF_BLOCK flip = {0};
    
    for(uint32_t y = 0; y != 4; y++)
    {
        for(uint32_t x = 0; x != 4; x++)
        {
            const uint32_t fidx = rgba_get_index(x, y, 4, 1);
            const uint32_t bidx = rgba_get_index(3-x, y, 4, 1);
            flip.data[fidx] = block.data[bidx];
        }
    }
    
    flip.hash = crc32_encode(&flip.data[0], 16);
    
    return flip;
}

/*
    Flips block vertically
*/
static inline EGIF_BLOCK egif_block_flip_vertical(EGIF_BLOCK block)
{
    EGIF_BLOCK flip = {0};
    
    for(uint32_t y = 0; y != 4; y++)
    {
        for(uint32_t x = 0; x != 4; x++)
        {
            const uint32_t fidx = rgba_get_index(x, y, 4, 1);
            const uint32_t bidx = rgba_get_index(x, 3-y, 4, 1);
            flip.data[fidx] = block.data[bidx];
        }
    }
    
    flip.hash = crc32_encode(&flip.data[0], 16);
    
    return flip;
}

/*
    Flips block both horizontally and vertically
*/
static inline EGIF_BLOCK egif_block_flip_both(EGIF_BLOCK block)
{
    EGIF_BLOCK flip = {0};
    
    for(uint32_t i = 0; i != 16; i++)
        flip.data[i] = block.data[15-i];
    
    flip.hash = crc32_encode(&flip.data[0], 16);
    
    return flip;
}

static inline EGIF_BLOCK_CODER egif_get_blocks_coder(EGIF_FILE* egif, const uint8_t* pixel_data,
                                                     const uint32_t x, const uint32_t y)
{
    EGIF_BLOCK_CODER coder;
    
    coder.blocks[0] = egif_get_block(egif, pixel_data, x, y);
    coder.blocks[1] = egif_block_flip_horizontal(coder.blocks[0]);
    coder.blocks[2] = egif_block_flip_vertical(coder.blocks[0]);
    coder.blocks[3] = egif_block_flip_both(coder.blocks[0]);

    return coder;
}

static inline uint8_t egif_check_if_block_exists(const uint32_t block_hash, uint32_t* index, uint8_t* orient,
                                                 EGIF_BLOCK_CODER* blocks, const uint32_t blocks_count)
{
    for(uint32_t i = 0; i != blocks_count; ++i)
    {
        for(uint8_t j = 0; j != 4; ++j)
        {
            if(block_hash == blocks[i].blocks[j].hash)
            {
                if(index) *index = i;
                if(orient) *orient = j;
                return 1;
            }
        }
    }

    return 0;
}

/*
    Returns amount of blocks generated
*/
static inline uint32_t egif_generate_blocks(EGIF_FILE* egif, const uint32_t width,
                                            const uint32_t height, const uint8_t* pixel_data)
{
    uint32_t bc = 0;
    uint32_t blocks_count_max = (width/4)*(height/4);
    egif->blocks = calloc(blocks_count_max, sizeof(EGIF_BLOCK_CODER));
    
    /* First block */
    egif->blocks[bc] = egif_get_blocks_coder(egif, pixel_data, 0, 0);
    
    /* Load blocks and discard duplicates */
    for(uint32_t y = 0; y != height; y+=4)
    {
        for(uint32_t x = 0; x != width; x+=4)
        {
            EGIF_BLOCK cur_block = egif_get_block(egif, pixel_data, x, y);
            const uint8_t is_present = egif_check_if_block_exists(cur_block.hash, NULL, NULL, egif->blocks, bc+1);
            
            if(is_present == 0)
            {
                bc += 1;
                egif->blocks[bc] = egif_get_blocks_coder(egif, pixel_data, x, y);
            }
        }
    }
    
    return (bc+1);
}

/*

*/
static inline void egif_generate_packets(EGIF_FILE* egif, const uint8_t* pixel_data)
{
    egif->packets = calloc((egif->width/4)*(egif->height/4), sizeof(EGIF_PACKET));
    
    uint32_t packet_id = 0;
    
    for(uint32_t y = 0; y != egif->height; y+=4)
    {
        for(uint32_t x = 0; x != egif->width; x+=4)
        {
            EGIF_PACKET* cur_packet = &egif->packets[packet_id];
            EGIF_BLOCK cur_block = egif_get_block(egif, pixel_data, x, y);
            uint32_t index = 0;
            uint8_t orient = 0;
            const uint8_t is_present = egif_check_if_block_exists(cur_block.hash, &index, &orient,
                                                                  egif->blocks, egif->blocks_count);
            
            if(is_present)
            {
                switch(egif->packet_size)
                {
                    case EGIF_PACKET_SMALL:
                        cur_packet->s.orient = orient;
                        cur_packet->s.block_id = index;
                        break;
                    case EGIF_PACKET_MEDIUM:
                        cur_packet->m.orient = orient;
                        cur_packet->m.block_id = index;
                        break;
                    case EGIF_PACKET_BIG:
                        cur_packet->b.orient = orient;
                        cur_packet->b.block_id = index;
                        break;
                }
                
                packet_id += 1;
            }
        }
    }
}

/*
    palette is 256 RGB24 values.
    pixel_data are the indices to palette entries.
    width and height should to be multiplies of 4
*/
static inline EGIF_FILE* egif_convert_from_data(const uint8_t* palette, const uint16_t palette_count,
                                                const uint32_t width, const uint32_t height,
                                                const uint8_t* pixel_data)
{
    EGIF_FILE* egif = calloc(1, sizeof(EGIF_FILE));
    
    egif->magic = EGIF_MAGIC;
    egif->width = width;
    egif->height = height;
    egif->palette_count = palette_count;
    
    /* Converting the palette from rgba32 to rgbas5551 */
    egif_pal_rgba_to_5551(egif, palette);
    
    /* Generate blocks */
    egif->blocks_count = egif_generate_blocks(egif, width, height, pixel_data);
    
    /* Realloc to minimize memory usage */
    EGIF_BLOCK_CODER* temp = calloc(egif->blocks_count, sizeof(EGIF_BLOCK_CODER));
    memcpy(&temp[0], &egif->blocks[0], sizeof(EGIF_BLOCK_CODER)*egif->blocks_count);
    free(egif->blocks);
    egif->blocks = temp;
    
    /* Figure out packet size */
    if(egif->blocks_count <= EGIF_PACKET_SMALL_MAX)
        egif->packet_size = EGIF_PACKET_SMALL;
    else if(egif->blocks_count <= EGIF_PACKET_MEDIUM_MAX)
        egif->packet_size = EGIF_PACKET_MEDIUM;
    else
        egif->packet_size = EGIF_PACKET_BIG;
    
    /* Generate packets */
    egif_generate_packets(egif, pixel_data);

    /* Done */
    return egif;
}

/*
    Frees data.
    Returns NULL.
*/
static inline EGIF_FILE* egif_free(EGIF_FILE* egif)
{
    free(egif->blocks);
    free(egif->packets);
    free(egif);
    return NULL;
}

static inline void egif_save_to_file(EGIF_FILE* egif, const char* filename)
{
    FILE* fegif = fopen(filename, "wb");
    
    if(fegif)
    {
        fwrite(&egif->magic, sizeof(egif->magic), 1, fegif);
        fwrite(&egif->width, sizeof(egif->width), 1, fegif);
        fwrite(&egif->height, sizeof(egif->height), 1, fegif);
        fwrite(&egif->blocks_count, sizeof(egif->blocks_count), 1, fegif);
        fwrite(&egif->packet_size, sizeof(egif->packet_size), 1, fegif);
        fwrite(&egif->palette_count, sizeof(egif->palette_count), 1, fegif);
        
        uint8_t* clut_buff = calloc(sizeof(RGBA5551)+1, egif->palette_count);
        egif->vrle_palette_size = vrle_compress(VRLE_TYPE_U16, (uint8_t*)&egif->palette[0],
                                                 sizeof(RGBA5551)*egif->palette_count,
                                                 &clut_buff[0]);
                                                 
        fwrite(&egif->vrle_palette_size, sizeof(egif->vrle_palette_size), 1, fegif);
        fwrite(clut_buff, egif->vrle_palette_size, 1, fegif);
        free(clut_buff);
       
        /* Compress blocks */
        uint8_t* blocks_buff = calloc(16, egif->blocks_count);
        uint8_t* blocksout_buff = calloc(16*2, egif->blocks_count);
       
        for(uint32_t i = 0; i != egif->blocks_count; ++i)
        {
            //fwrite(&egif->blocks[i].blocks[0].data[0], 16, 1, fegif);
            memcpy(&blocks_buff[i*16], &egif->blocks[i].blocks[0].data[0], 16);
        }
        
        egif->vrle_blocks_size = vrle_compress(VRLE_TYPE_U16, (uint8_t*)&blocks_buff[0],
                                               egif->blocks_count*16,
                                               &blocksout_buff[0]);
                                                 
        fwrite(&egif->vrle_blocks_size, sizeof(egif->vrle_blocks_size), 1, fegif);
        fwrite(blocksout_buff, egif->vrle_blocks_size, 1, fegif);
        
        free(blocks_buff);
        free(blocksout_buff);
        
        /* Compress packets */
        uint8_t* packet_buff = calloc(egif->packet_size+1, (egif->width/4)*(egif->height/4));
        uint8_t* packetout_buff = calloc(egif->packet_size+2, (egif->width/4)*(egif->height/4));
        
        for(uint32_t i = 0; i != ((egif->width/4)*(egif->height/4)); ++i)
        {
            switch(egif->packet_size)
            {
                case EGIF_PACKET_SMALL:
                    //fwrite(&egif->packets[i].s, 1, 1, fegif);
                    memcpy(&packet_buff[i], &egif->packets[i].s, 1);
                    break;
                case EGIF_PACKET_MEDIUM:
                    //fwrite(&egif->packets[i].m, 2, 1, fegif);
                    memcpy(&packet_buff[i*2], &egif->packets[i].m, 2);
                    break;
                case EGIF_PACKET_BIG:
                    //fwrite(&egif->packets[i].b, 3, 1, fegif);
                    memcpy(&packet_buff[i*3], &egif->packets[i].b, 3);
                    break;
            }
        }

        egif->vrle_packets_size = vrle_compress(egif->packet_size, (uint8_t*)&packet_buff[0],
                                                ((egif->width/4)*(egif->height/4))*(egif->packet_size+1),
                                                &packetout_buff[0]);
                                                 
        fwrite(&egif->vrle_packets_size, sizeof(egif->vrle_packets_size), 1, fegif);
        fwrite(packetout_buff, egif->vrle_packets_size, 1, fegif);
        free(packet_buff);
        free(packetout_buff);
        
        fclose(fegif);
    }
}

static inline EGIF_FILE* egif_load_from_file(const char* filename)
{
    EGIF_FILE* egif = calloc(1, sizeof(EGIF_FILE));
    FILE* fegif = fopen(filename, "rb");
    
    if(fegif && egif)
    {
        fread(&egif->magic, sizeof(egif->magic), 1, fegif);
        fread(&egif->width, sizeof(egif->width), 1, fegif);
        fread(&egif->height, sizeof(egif->height), 1, fegif);
        fread(&egif->blocks_count, sizeof(egif->blocks_count), 1, fegif);
        fread(&egif->packet_size, sizeof(egif->packet_size), 1, fegif);
        fread(&egif->palette_count, sizeof(egif->palette_count), 1, fegif);
        
        /* Decompress CLUT */
        fread(&egif->vrle_palette_size, sizeof(egif->vrle_palette_size), 1, fegif);
        uint8_t* clut_buff = calloc(1, egif->vrle_palette_size);
        fread(&clut_buff[0], 1, egif->vrle_palette_size, fegif);
        vrle_decompress(clut_buff, egif->vrle_palette_size, (uint8_t*)&egif->palette[0]);
        free(clut_buff);
        
        /* Decompress Blocks */
        egif->blocks = calloc(egif->blocks_count, sizeof(EGIF_BLOCK_CODER));
        fread(&egif->vrle_blocks_size, sizeof(egif->vrle_blocks_size), 1, fegif);
        
        uint8_t* blocks_buff = calloc(1, egif->vrle_blocks_size);
        uint8_t* blocksout_buff = calloc(16, egif->blocks_count);
        
        fread(blocks_buff, egif->vrle_blocks_size, 1, fegif);
        vrle_decompress(blocks_buff, egif->vrle_blocks_size, (uint8_t*)&blocksout_buff[0]);
        
        free(blocks_buff);
        
        for(uint32_t i = 0; i != egif->blocks_count; ++i)
        {
            //fread(&egif->blocks[i].blocks[0].data, 16, 1, fegif);
            memcpy(&egif->blocks[i].blocks[0].data, &blocksout_buff[i*16], 16); 
            egif->blocks[i].blocks[1] = egif_block_flip_horizontal(egif->blocks[i].blocks[0]);
            egif->blocks[i].blocks[2] = egif_block_flip_vertical(egif->blocks[i].blocks[0]);
            egif->blocks[i].blocks[3] = egif_block_flip_both(egif->blocks[i].blocks[0]);
        }
        
        free(blocksout_buff);
        
        /* Decompress Packets */
        const uint32_t packet_count = (egif->width/4)*(egif->height/4);
        egif->packets = calloc(packet_count, sizeof(EGIF_PACKET));
        
        fread(&egif->vrle_packets_size, sizeof(egif->vrle_packets_size), 1, fegif);
        
        uint8_t* packets_buff = calloc(1, egif->vrle_packets_size);
        uint8_t* packetsout_buff = calloc(sizeof(EGIF_PACKET), packet_count);
        
        fread(packets_buff, egif->vrle_packets_size, 1, fegif);
        vrle_decompress(packets_buff, egif->vrle_packets_size, (uint8_t*)&packetsout_buff[0]);
        
        free(packets_buff);
        
        for(uint32_t i = 0; i != packet_count; ++i)
        {
            switch(egif->packet_size)
            {
                case EGIF_PACKET_SMALL:
                    //fread(&egif->packets[i].s, 1, 1, fegif);
                    memcpy(&egif->packets[i].s, &packetsout_buff[i], 1);
                    break;
                case EGIF_PACKET_MEDIUM:
                    //fread(&egif->packets[i].m, 2, 1, fegif);
                    memcpy(&egif->packets[i].s, &packetsout_buff[i*2], 2);
                    break;
                case EGIF_PACKET_BIG:
                    //fread(&egif->packets[i].b, 3, 1, fegif);
                    memcpy(&egif->packets[i].s, &packetsout_buff[i*3], 3);
                    break;
            }
        }
        
        free(packetsout_buff);
        
        fclose(fegif);
    }
    
    return egif;
}

static inline void egif_copy_block_to_rgb(EGIF_FILE* egif, EGIF_BLOCK block,
                                          uint32_t x, uint32_t y,
                                          uint8_t* buffer)
{
    for(uint32_t by = 0; by != 4; by++)
    {
        for(uint32_t bx = 0; bx != 4; bx++)
        {
            const uint32_t block_index = rgba_get_index(bx, by, 4, 1);
            const uint32_t buffer_index = rgba_get_index(x+bx, y+by, egif->width, 3);
            RGB888 col = rgb_unpack_5551(egif->palette[block.data[block_index]]);
            buffer[buffer_index] = col.r;
            buffer[buffer_index+1] = col.g;
            buffer[buffer_index+2] = col.b;
        }
    }
}

static inline uint8_t* egif_to_raw_rgb888(EGIF_FILE* egif)
{
    uint32_t cpi = 0;
    uint8_t* buffer = calloc(egif->width*egif->height*3, 1);

    for(uint32_t y = 0; y != egif->height; y+=4)
    {
        for(uint32_t x = 0; x != egif->width; x+=4)
        {
            uint8_t orient = 0;
            uint32_t block_id = 0;
            
            switch(egif->packet_size)
            {
                case EGIF_PACKET_SMALL:
                    orient = egif->packets[cpi].s.orient;
                    block_id = egif->packets[cpi].s.block_id;
                    break;
                case EGIF_PACKET_MEDIUM:
                    orient = egif->packets[cpi].m.orient;
                    block_id = egif->packets[cpi].m.block_id;
                    break;
                case EGIF_PACKET_BIG:
                    orient = egif->packets[cpi].b.orient;
                    block_id = egif->packets[cpi].b.block_id;
                    break;
            }
            
            EGIF_BLOCK cb = egif->blocks[block_id].blocks[orient];
            egif_copy_block_to_rgb(egif, cb, x, y, buffer);
            
            cpi += 1;
        }
    }
    
    return buffer;
}