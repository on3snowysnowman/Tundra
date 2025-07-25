
#include <cstdlib>
#include <ctime>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "tundra/utils/Memory.hpp"

constexpr uint8_t ALIGNMENT = 32;
constexpr uint8_t NUM_BYTE_DOUBLE = 30;
constexpr uint16_t ITERATIONS = 20;

uint32_t total_time_std[NUM_BYTE_DOUBLE];
uint32_t total_time_tundra[NUM_BYTE_DOUBLE];
uint32_t num_bytes_store[NUM_BYTE_DOUBLE];


void benchmark_copy()
{
    for(int i = 1; i < NUM_BYTE_DOUBLE + 1; ++i)
    {
        uint32_t num_bytes = 256 * pow(i, 2);

        num_bytes_store[i - 1] = num_bytes;

        // uint8_t *bytes = (uint8_t*)malloc(j);
        // uint8_t *dest = (uint8_t*)malloc(j);
        
        uint8_t *bytes, *dest;
    
        posix_memalign((void**)&bytes, ALIGNMENT, num_bytes);
        posix_memalign((void**)&dest, ALIGNMENT, num_bytes);
    
        for(int j = 0; j < num_bytes; ++j)
        {
            bytes[j] = rand() % 255;
        }
    
        timespec start, end;
    
        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    
        memcpy(bytes, dest, num_bytes);
    
        clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    
        // Elapsed nano seconds. (10^-9 seconds)
        uint32_t elapsed_std = (end.tv_sec - start.tv_sec) + 
                    (end.tv_nsec - start.tv_nsec);
        
        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    
        Tundra::copy_aligned_mem<ALIGNMENT>(bytes, dest, num_bytes);
    
        clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    
        // Elapsed nano seconds. (10^-9 seconds)
        uint32_t elapsed_tundra = (end.tv_sec - start.tv_sec) + 
                    (end.tv_nsec - start.tv_nsec);
    
        // printf("STD [%d]: %d\n", j, elapsed_std);
        // printf("Tundra[%d]: %d\n", j, elapsed_tundra);
        // printf("Tundra / STD: %.1f%%\n\n", ((double)elapsed_tundra / elapsed_std) * 100);
    
        total_time_std[i - 1] += elapsed_std;
        total_time_tundra[i - 1] += elapsed_tundra;

        free(bytes);
        free(dest);
        
    }

}


int main()
{
    srand(time(NULL));

    for(int i = 0; i < NUM_BYTE_DOUBLE; ++i)
    {
        total_time_std[i] = 0;
        total_time_tundra[i] = 0;
    }

    for(int i = 0; i < ITERATIONS; ++i)
    {
        benchmark_copy();
    }

    for(int i = 0; i < NUM_BYTE_DOUBLE; ++i)
    {
        printf("Num bytes: %d\n", num_bytes_store[i]);
        printf("Tundra: %d\n", total_time_tundra[i] / ITERATIONS);
        printf("Std: %d\n", total_time_std[i] / ITERATIONS);
        printf("Tundra / Std: %d%%\n\n", int(((double)total_time_tundra[i] /
            total_time_std[i]) * 100));
    }

    return 0;
}