#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "vrle.h"

uint64_t get_file_size(FILE* f);
void compress_data_vrle(const uint8_t type, uint8_t* file_data, const uint32_t data_size, FILE* fout);

int main(int argc, char** argv)
{
    if(argc < 3) return 0;
    
    FILE* fin = fopen(argv[2], "rb");
    const uint32_t file_size = get_file_size(fin);
    uint8_t* file_data = calloc(file_size, 1);
    fread(file_data, file_size, 1, fin);
    fclose(fin);
    
    FILE* fout = fopen("output.bin", "wb");

    switch(argv[1][0])
    {
        case '0': compress_data_vrle(VRLE_TYPE_U8,  file_data, file_size, fout); break;
        case '1': compress_data_vrle(VRLE_TYPE_U16, file_data, file_size, fout); break;
        case '2': compress_data_vrle(VRLE_TYPE_U24, file_data, file_size, fout); break;
        case '3': compress_data_vrle(VRLE_TYPE_U32, file_data, file_size, fout); break;
    }
    
    free(file_data);
    fclose(fout);
    
    return 0;
}

uint64_t get_file_size(FILE* f)
{
    const uint64_t posold = ftell(f);
    fseek(f, 0, SEEK_END);
    const uint64_t pos = ftell(f);
    fseek(f, posold, SEEK_SET);
    return pos;
}

void compress_data_vrle(const uint8_t type, uint8_t* file_data, const uint32_t data_size, FILE* fout)
{
    uint32_t data_rem = data_size;
    
    /* VRLE the data */
    while(data_rem)
    {
        VRLE_HEADER head = {0};
        uint8_t* data_ptr = &file_data[data_size-data_rem];
        
        if(data_rem < (type+1))
        {
            head = vrle_count_packets(data_rem-1, data_ptr, data_rem);
        }
        else
        {
            head = vrle_count_packets(type, data_ptr, data_rem);
        }

        fwrite(&head, 1, 1, fout);
        fwrite(data_ptr, (head.type+1), 1, fout);

        data_rem -= head.len*(head.type+1);
    }
}