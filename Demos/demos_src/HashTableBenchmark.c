
/**
 * Basic benchmark test for the HashTable
*/

#define _POSIX_C_SOURCE 199309L
#define TUNDRA_BENCHMARKING

#include <stdint.h>

#ifndef TUNDRA_HASHTABLE_UINT8_UINT8
#define TUNDRA_HASHTABLE_UINT8_UINT8
#define TUNDRA_HSHTBL_KEYTYPE uint8_t
#define TUNDRA_HSHTBL_VALUETYPE uint8_t
#define TUNDRA_HSHTBL_NAME UInt8UInt8
#include "tundra/tundra_utils/HashTable.h"
#undef TUNDRA_HSHTBL_KEYTYPE
#undef TUNDRA_HSHTBL_VALUETYPE
#endif

#define TUNDRA_DYNARR_TYPE uint8_t
#define TUNDRA_DYNARR_NAME UInt8
#include "tundra/tundra_utils/DynamicArray.h"
#undef TUNDRA_DYNARR_TYPE
#undef TUNDRA_DYNARR_NAME

#include <stdio.h>
#include <stdlib.h>
#include <time.h>


uint64_t total_collided = 0; 
uint64_t total_added = 0;
uint64_t total_resizes = 0;

float proportion_lowest_time = 100.0f;
float limit_lowest_time = 100.0f;
double second_lowest_time = 100.0;
double lowest_time = 100.0;

const uint64_t ITERATIONS = 10000;

Tundra_DynamicArrayUInt8 input_data;

void test_table(int seed)
{
    srand(seed);

    Tundra_HashTableUInt8UInt8 table;

    Tundra_HshTblUInt8UInt8_init(&table);

    for(int i = 0; i < 100; ++i)
    {
        // Tundra_HshTblUInt8UInt8_add(&table, (uint8_t[]){rand() % 255},
        //     (uint8_t[]){rand() % 255});
        Tundra_HshTblUInt8UInt8_add(&table, &input_data.data[i], 
            &input_data.data[i]);
    }

    Tundra_HshTblUInt8UInt8_deconstruct(&table);
}


int main()
{
    Tundra_DynArrUInt8_init(&input_data, 100);

    for(int i = 0; i < 100; ++i)
    {
        Tundra_DynArrUInt8_add(&input_data, (uint8_t[]){rand() % 255});
    }

    struct timespec start, end;

    puts("");

    for(int j = 3; j < 10; ++j)
    {
        TUNDRA_HSHTBL_TOP_LIMIT = (float)j / 10;

        for(int f = 1; f < 10; ++f)
        {
            TUNDRA_HSHTBL_TOP_PROPORTION = (float)f / 10;
    
            total_collided = 0;
            total_added = 0;
            total_resizes = 0;

            clock_gettime(CLOCK_MONOTONIC, &start);
    
            for(uint64_t i = 0; i < ITERATIONS; ++i)
            {
                test_table(i);
            }

            clock_gettime(CLOCK_MONOTONIC, &end);

            double elapsed_time = (end.tv_sec - start.tv_sec)
                    + (end.tv_nsec - start.tv_nsec) / 1e9;

            if(elapsed_time < lowest_time)
            {
                second_lowest_time = lowest_time;
                lowest_time = elapsed_time;
                proportion_lowest_time = TUNDRA_HSHTBL_TOP_PROPORTION;
                limit_lowest_time = TUNDRA_HSHTBL_TOP_LIMIT;
            }
    
            printf("Proportion: %f\nLimit: %f\n", TUNDRA_HSHTBL_TOP_PROPORTION,
                TUNDRA_HSHTBL_TOP_LIMIT);
            printf("Elapsed: %f\n\n", elapsed_time);
        }
    }


    printf("Proportion with Lowest Time: %f\n", proportion_lowest_time);
    printf("Limit with Lowest Time: %f\n\n", limit_lowest_time);
    printf("Lowest Time: %f\n", lowest_time);
    printf("Second Lowest Time: %f\n", second_lowest_time);

    return 0;

}
