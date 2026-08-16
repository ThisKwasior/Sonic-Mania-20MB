#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "miniz.h"

uint64_t get_file_size(FILE* f);

int main(int argc, char** argv)
{
    FILE* fin = fopen(argv[1], "rb");
    const uint32_t fin_size = get_file_size(fin);
    uint8_t* fin_data = calloc(fin_size, 1);
    fread(fin_data, fin_size, 1, fin);
    fclose(fin);
    
    mz_ulong fout_size = fin_size*2;
    uint8_t* fout_data = calloc(fin_size*2, 1);
    
    mz_compress2(fout_data, &fout_size, fin_data, fin_size, MZ_UBER_COMPRESSION);
    
    FILE* fout = fopen(argv[2], "wb");
    fprintf(fout, "CMPZ");
    fwrite(&fin_size, 4, 1, fout);
    uint32_t fout_size_u32 = fout_size;
    fwrite(&fout_size_u32, 4, 1, fout);
    fwrite(fout_data, fout_size, 1, fout);
    fclose(fout);
    
    free(fin_data);
    free(fout_data);
    
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