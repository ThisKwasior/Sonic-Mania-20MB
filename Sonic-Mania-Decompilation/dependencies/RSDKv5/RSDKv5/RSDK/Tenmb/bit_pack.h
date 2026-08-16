#pragma once

#include <stdint.h>

#define VEC3_S32_STEP (8.f)
#define VEC3_S32_FRAC (1/8.f)

typedef struct 
{
    int8_t x : 3;
    int8_t y : 2;
    int8_t z : 3;
} PACK_VEC3_NRM_S8;

typedef struct 
{
    int8_t x;
    int8_t y;
    int8_t z;
} PACK_VEC3_NRM_S24;

typedef struct 
{
    int16_t x : 5;
    int16_t y : 6;
    int16_t z : 5;
} PACK_VEC3_S16;

typedef PACK_VEC3_S16 PACK_VEC3_NRM_S16;

typedef struct 
{
    int32_t x : 11;
    int32_t y : 11;
    int32_t z : 10;
} PACK_VEC3_S32;

static inline PACK_VEC3_NRM_S8 f32_to_nrm_vec8(float x, float y, float z)
{
    PACK_VEC3_NRM_S8 vec;
    vec.x = x*3;
    vec.y = y*1;
    vec.z = z*3;
    return vec;
}

static inline void packed_nrm_vec8_to_f32(PACK_VEC3_NRM_S8 vec, float* x, float* y, float* z)
{
    *x = vec.x/3.f;
    *y = vec.y/1.f;
    *z = vec.z/3.f;
}

static inline PACK_VEC3_NRM_S24 f32_to_nrm_vec24(float x, float y, float z)
{
    PACK_VEC3_NRM_S24 vec;
    vec.x = x*127;
    vec.y = y*127;
    vec.z = z*127;
    return vec;
}

static inline void packed_nrm_vec24_to_f32(PACK_VEC3_NRM_S24 vec, float* x, float* y, float* z)
{
    *x = vec.x/127.f;
    *y = vec.y/127.f;
    *z = vec.z/127.f;
}

static inline PACK_VEC3_NRM_S16 f32_to_nrm_vec16(float x, float y, float z)
{
    PACK_VEC3_NRM_S16 vec;
    vec.x = x*15;
    vec.y = y*31;
    vec.z = z*15;
    return vec;
}

static inline void packed_nrm_vec16_to_f32(PACK_VEC3_NRM_S16 vec, float* x, float* y, float* z)
{
    *x = vec.x/15.f;
    *y = vec.y/31.f;
    *z = vec.z/15.f;
}

static inline PACK_VEC3_S16 f32_to_packed_vec16(float x, float y, float z)
{
    PACK_VEC3_S16 vec;
    vec.x = (x*16);
    vec.y = (y*32);
    vec.z = (z*16);
    return vec;
}

static inline void packed_vec16_to_f32(PACK_VEC3_S16 vec, float* x, float* y, float* z)
{
    *x = vec.x/16.f;
    *y = vec.y/32.f;
    *z = vec.z/16.f;
}

static inline PACK_VEC3_S32 f32_to_packed_vec32(float x, float y, float z)
{
    PACK_VEC3_S32 vec;
    vec.x = (x*VEC3_S32_STEP);
    vec.y = (y*VEC3_S32_STEP);
    vec.z = (z*VEC3_S32_STEP);
    return vec;
}

static inline void packed_vec32_to_f32(PACK_VEC3_S32 vec, float* x, float* y, float* z)
{
    *x = (vec.x/VEC3_S32_STEP);
    *y = (vec.y/VEC3_S32_STEP);
    *z = (vec.z/VEC3_S32_STEP);
}