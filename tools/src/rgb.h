#pragma once

typedef struct 
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
} RGB888;

typedef struct 
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} RGBA8888;

typedef struct 
{
    uint16_t r : 5;
    uint16_t g : 6;
    uint16_t b : 5;
} RGB565;

typedef struct 
{
    uint16_t r : 5;
    uint16_t g : 5;
    uint16_t b : 5;
    uint16_t a : 1;
} RGBA5551;

static inline RGB565 rgb_to_565_comp(const uint8_t r, const uint8_t g, const uint8_t b)
{
    RGB565 col;
    col.r = r>>3;
    col.g = g>>2;
    col.b = b>>3;
    return col;
}

static inline RGB888 rgb565_to_888(const RGB565 col)
{
    RGB888 col2;
    col2.r = col.r<<3;
    col2.g = col.g<<2;
    col2.b = col.b<<3;
    return col2;
}

static inline RGBA5551 rgba_to_5551(RGBA8888 col)
{
    RGBA5551 col2;
    col2.r = col.r>>3;
    col2.g = col.g>>3;
    col2.b = col.b>>3;
    col2.a = col.a > 127 ? 1 : 0;
    return col2;
}

static inline RGBA5551 rgba_888_to_5551(RGB888 col)
{
    RGBA5551 col2;
    col2.r = col.r>>3;
    col2.g = col.g>>3;
    col2.b = col.b>>3;
    col2.a = 1;
    return col2;
}

static inline RGB888 rgb_unpack_5551(RGBA5551 col)
{
    RGB888 col2;
    col2.r = col.r<<3;
    col2.g = col.g<<3;
    col2.b = col.b<<3;
    return col2;
}

static inline RGBA8888 rgba_unpack_5551(RGBA5551 col)
{
    RGBA8888 col2;
    col2.r = col.r<<3;
    col2.g = col.g<<3;
    col2.b = col.b<<3;
    col2.a = col.a ? 255 : 0;
    return col2;
}

static inline uint32_t rgba_get_index(const uint32_t x, const uint32_t y,
                                      const uint32_t w, const uint8_t n)
{
    return (((w*n)*y)+(x*n));
}