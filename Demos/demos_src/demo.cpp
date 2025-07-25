
#include <cstdlib>
#include <ctime>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "tundra/utils/Memory.hpp"

constexpr uint8_t ALIGNMENT = 32;
constexpr uint8_t NUM_BYTE_DOUBLE = 10;
constexpr uint16_t ITERATIONS = 25;

uint32_t total_time_std[NUM_BYTE_DOUBLE];
uint32_t total_time_tundra[NUM_BYTE_DOUBLE];
uint32_t num_bytes_store[NUM_BYTE_DOUBLE];


void benchmark_copy()
{
    for(int i = 1; i < NUM_BYTE_DOUBLE + 1; ++i)
    {
        uint32_t num_bytes = 256 * pow(2, i);

        num_bytes_store[i - 1] = num_bytes;

        // uint8_t *bytes = (uint8_t*)malloc(j);
        // uint8_t *dest = (uint8_t*)malloc(j);
        
        uint8_t *bytes_tundra, *dest_tundra, *bytes_std, *dest_std;
        uint32_t elapsed_std, elapsed_tundra;
    
        // posix_memalign((void**)&bytes_tundra, ALIGNMENT, num_bytes);
        // posix_memalign((void**)&dest_tundra, ALIGNMENT, num_bytes);
        // posix_memalign((void**)&bytes_std, ALIGNMENT, num_bytes);
        // posix_memalign((void**)&dest_std, ALIGNMENT, num_bytes);
        
        bytes_tundra = (uint8_t*)malloc(num_bytes);
        dest_tundra = (uint8_t*)malloc(num_bytes);
        bytes_std = (uint8_t*)malloc(num_bytes);
        dest_std = (uint8_t*)malloc(num_bytes);

        for(int j = 0; j < num_bytes; ++j)
        {
            bytes_tundra[j] = rand() % 255;
            bytes_std[j] = rand() % 255;
        }

        memset(dest_std, 0, num_bytes);
        memset(dest_tundra, 0, num_bytes);
        __builtin_prefetch(bytes_std, 0, 3);
        __builtin_prefetch(bytes_tundra, 0, 3);

        int loop_count = (1000 > 1'048'576 / num_bytes) ? 1000 : 1'048'576 / num_bytes;
    
        timespec start, end;
    
        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    
        for (int i = 0; i < loop_count; ++i)
            // memcpy(dest_std, bytes_std, num_bytes);
            Tundra::copy_mem(bytes_tundra, dest_tundra, num_bytes);

    
        clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    
        // Elapsed nano seconds. (10^-9 seconds)
        elapsed_tundra = (end.tv_sec - start.tv_sec) * 1000000000UL + 
             (end.tv_nsec - start.tv_nsec);

        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    
        for (int i = 0; i < loop_count; ++i)
            memcpy(dest_std, bytes_std, num_bytes);
    
        clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    
        // Elapsed nano seconds. (10^-9 seconds)
        elapsed_std = (end.tv_sec - start.tv_sec) * 1000000000UL + 
             (end.tv_nsec - start.tv_nsec);
    
        // printf("STD [%d]: %d\n", j, elapsed_std);
        // printf("Tundra[%d]: %d\n", j, elapsed_tundra);
        // printf("Tundra / STD: %.1f%%\n\n", ((double)elapsed_tundra / elapsed_std) * 100);
    
        total_time_std[i - 1] += elapsed_std;
        total_time_tundra[i - 1] += elapsed_tundra;

        free(bytes_std);
        free(dest_std);
        free(bytes_tundra);
        free(dest_tundra);
        
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