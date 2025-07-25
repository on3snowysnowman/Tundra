
#include <cstdlib>
#include <ctime>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "tundra/utils/Memory.hpp"

constexpr uint8_t ALIGNMENT = 32;

int main()
{
    srand(time(NULL));

    for(int j = 256; j < 100000; j *= 2)
    {
        // uint8_t *bytes = (uint8_t*)malloc(j);
        // uint8_t *dest = (uint8_t*)malloc(j);

        uint8_t *bytes, *dest;

        posix_memalign((void**)&bytes, ALIGNMENT, j);
        posix_memalign((void**)&dest, ALIGNMENT, j);

        for(int i = 0; i < j; ++i)
        {
            bytes[i] = rand() % 255;
        }

        timespec start, end;

        clock_gettime(CLOCK_MONOTONIC_RAW, &start);

        memcpy(bytes, dest, j);

        clock_gettime(CLOCK_MONOTONIC_RAW, &end);

        // Elapsed nano seconds. (10^-9 seconds)
        uint32_t elapsed_std = (end.tv_sec - start.tv_sec) + 
                    (end.tv_nsec - start.tv_nsec);
        
        clock_gettime(CLOCK_MONOTONIC_RAW, &start);

        Tundra::copy_aligned_mem<ALIGNMENT>(bytes, dest, j);

        clock_gettime(CLOCK_MONOTONIC_RAW, &end);

        // Elapsed nano seconds. (10^-9 seconds)
        uint32_t elapsed_tundra = (end.tv_sec - start.tv_sec) + 
                    (end.tv_nsec - start.tv_nsec);

        printf("STD [%d]: %d\n", j, elapsed_std);
        printf("Tundra[%d]: %d\n", j, elapsed_tundra);
        printf("Tundra / STD: %.1f%%\n\n", ((double)elapsed_tundra / elapsed_std) * 100);

        free(bytes);
        free(dest);
    }

    return 0;
}