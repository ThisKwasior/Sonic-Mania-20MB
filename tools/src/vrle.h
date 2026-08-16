#pragma once

#include <stdint.h>
#include <string.h>

#define VRLE_TYPE_U8    (0)
#define VRLE_TYPE_U16   (1)
#define VRLE_TYPE_U24   (2)
#define VRLE_TYPE_U32   (3)
#define VRLE_MAX_LEN    (63)

typedef union
{
    uint32_t u8 : 8;
    uint32_t u16 : 16;
    uint32_t u24 : 24;
    uint32_t u32;
} VRLE_DATA;

typedef struct
{
    uint8_t type : 2;
    uint8_t len  : 6;
} VRLE_HEADER;

typedef struct
{
    VRLE_HEADER head;
    VRLE_DATA data;
} VRLE;

static inline VRLE_HEADER vrle_count_packets(const uint8_t vrle_type, const uint8_t* data, const uint32_t data_size)
{
    VRLE_HEADER head = {0};
    const uint32_t len_for_type = (data_size/(vrle_type+1));
    
    /* No data available for current type */
    if(len_for_type == 0)
        return head;
    
    const uint32_t available = len_for_type < (VRLE_MAX_LEN) ? len_for_type : (VRLE_MAX_LEN);
    uint8_t count = 1;
    uint32_t value = 0;
    uint32_t p = 0;

    switch(vrle_type)
    {
        case VRLE_TYPE_U16: value = *(uint16_t*)&data[0];               break;
        case VRLE_TYPE_U24: value = ((*(uint32_t*)&data[0])&0xFFFFFF);  break;
        case VRLE_TYPE_U32: value = *(uint32_t*)&data[0];               break;
        default:            value = data[0];    
    }
    
    for(uint8_t i = 1; i != available; ++i)
    {
        uint32_t cur_value = 0;
        
        switch(vrle_type)
        {
            case VRLE_TYPE_U16: cur_value = *(uint16_t*)&data[i*2];                 break;
            case VRLE_TYPE_U24: cur_value = ((*(uint32_t*)&data[i*3])&0xFFFFFF);    break;
            case VRLE_TYPE_U32: cur_value = *(uint32_t*)&data[i*4];                 break;
            default:            cur_value = data[i];
        }
        
        if(value == cur_value)
            count += 1;
        else
            break;
    }

    head.type = vrle_type;
    head.len = count;
    return head;
}

/*
    Returns size of compressed data
*/
static inline uint32_t vrle_compress(const uint8_t vrle_type, const uint8_t* data, const uint32_t data_size, uint8_t* buffer)
{
    uint32_t rem = data_size;
    uint32_t buffer_pos = 0;
    
    while(rem)
    {
        const uint32_t data_pos = data_size-rem;
        VRLE_HEADER head = vrle_count_packets(vrle_type, &data[data_pos], rem);
        uint32_t cur_value = 0;
        
        switch(vrle_type)
        {
            case VRLE_TYPE_U16: cur_value = *(uint16_t*)&data[data_pos];                 break;
            case VRLE_TYPE_U24: cur_value = ((*(uint32_t*)&data[data_pos])&0xFFFFFF);    break;
            case VRLE_TYPE_U32: cur_value = *(uint32_t*)&data[data_pos];                 break;
            default:            cur_value = data[data_pos];
        }
        
        buffer[buffer_pos] = *(uint8_t*)&head;
        memcpy(&buffer[buffer_pos+1], &cur_value, head.type+1);
        
        rem -= head.len*(head.type+1);
        buffer_pos += 1 + (head.type+1);
    }
    
    return buffer_pos;
}

/*
*/
static inline void vrle_decompress(uint8_t* data, const uint32_t data_size, uint8_t* buffer)
{
    uint32_t rem = data_size;
    uint32_t buf_pos = 0;
    
    while(rem)
    {
        const uint32_t data_pos = data_size-rem;
        VRLE_HEADER head = *(VRLE_HEADER*)&data[data_pos];
        const uint8_t type_size = head.type+1;
        uint32_t cur_value = 0;
        
        switch(head.type)
        {
            case VRLE_TYPE_U16: cur_value = *(uint16_t*)&data[data_pos+1];                 break;
            case VRLE_TYPE_U24: cur_value = ((*(uint32_t*)&data[data_pos+1])&0xFFFFFF);    break;
            case VRLE_TYPE_U32: cur_value = *(uint32_t*)&data[data_pos+1];                 break;
            default:            cur_value = data[data_pos+1];
        }
        
        for(uint32_t i = 0; i != head.len; ++i)
        {
            memcpy(&buffer[buf_pos], &cur_value, type_size);
            buf_pos += type_size;
        }
        
        rem -= (1+type_size);
    }
}