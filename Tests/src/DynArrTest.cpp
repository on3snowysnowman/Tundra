/**
 * @file DynArrTest.cpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Test file for testing the DynamicArray container.
 * @date 2025-11-21
 *
 * @copyright Copyright (c) 2025
 */

#include <iostream>
#include <cassert>
#include <random>

#include "../TestingHelper.hpp"
#include "tundra/Tundra.h"
#include "tundra/utils/Math.h"
#include "tundra/containers/DynamicArrayint.h"

#define CALC_CAP_BYTES(num_elem) \
    Tundra_ceil_pow2(num_elem * sizeof(int))

static constexpr int TEST_ITERATIONS = 50;

/**
 * @brief Returns a random integer between `min` and `max`.
 * 
 * @param min 
 * @param max 
 * @return int 
 */
int get_rand_int(int min, int max)
{
    static std::mt19937 rng{ std::random_device{}() };
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

TEST_BEGIN(basic_init)
{
    Tundra_DynamicArrayint arr; 
    Tundra_DynArrint_init(&arr);

    assert(arr.cap == TUNDRA_DYNARR_DEF_CAP);
    assert(arr.cap_bytes ==
        Tundra_ceil_pow2(TUNDRA_DYNARR_DEF_CAP * sizeof(int)));
    assert(arr.num_elem == 0);
    assert(arr.data);

    Tundra_DynArrint_free(&arr);
}
TEST_END

TEST_BEGIN(cap_init)
{
    for(int i = 0; i < TEST_ITERATIONS; ++i)
    {
        int capacity = get_rand_int(1, 33);

        Tundra_DynamicArrayint arr;
        Tundra_DynArrint_init_w_cap(&arr, capacity);

        int expected_cap_bytes = Tundra_ceil_pow2(capacity * sizeof(int));

        assert(arr.cap == expected_cap_bytes / sizeof(int));
        assert(arr.cap_bytes == expected_cap_bytes);
        assert(arr.num_elem == 0);
        assert(arr.data);

        Tundra_DynArrint_free(&arr);
    }
}
TEST_END

TEST_BEGIN(elem_init)
{
    for(int i = 0; i < TEST_ITERATIONS; ++i)
    {
        // Init initial elements
        int num_elem = get_rand_int(1, 15);
        int *init_elems = new int[num_elem];

        for(int j = 0; j < num_elem; ++j)
        {
            init_elems[j] = get_rand_int(-100, 100);
        }

        // Init Array
        Tundra_DynamicArrayint arr;
        Tundra_DynArrint_init_w_elems(&arr, init_elems, num_elem);

        // Test correctness
        int expected_cap_bytes = CALC_CAP_BYTES(num_elem);

        assert(arr.cap == expected_cap_bytes / sizeof(int));
        assert(arr.cap_bytes == expected_cap_bytes);
        assert(arr.num_elem == num_elem);
        assert(arr.data);

        // Check each elements
        for(int j = 0; j < num_elem; ++j)
        {
            assert(arr.data[j] == init_elems[j]);
        }

        Tundra_DynArrint_free(&arr);

        delete[] init_elems;
    }

}
TEST_END

TEST_BEGIN(copy_init)
{
    for(int i = 0; i < TEST_ITERATIONS; ++i)
    {
        // Initialize initial elements
        int num_elem = get_rand_int(1, 15);
        int *init_elems = new int[num_elem];

        for(int j = 0; j < num_elem; ++j)
        {
            init_elems[j] = get_rand_int(-100, 100);
        }

        // Init source Array
        Tundra_DynamicArrayint src;
        Tundra_DynArrint_init_w_elems(&src, init_elems, num_elem);

        // Init dest Array
        Tundra_DynamicArrayint dest;
        Tundra_DynArrint_init_w_copy(&src, &dest);

        // Test correctness
        assert(dest.cap == src.cap);
        assert(dest.cap_bytes == src.cap_bytes);
        assert(dest.num_elem == src.num_elem);
        assert(dest.data && dest.data != src.data);

        // Check each element
        for(int j = 0; j < num_elem; j++)
        {
            assert(dest.data[j] == src.data[j]);
        }

        Tundra_DynArrint_free(&src);
        Tundra_DynArrint_free(&dest);

        delete[] init_elems;
    }

}
TEST_END

TEST_BEGIN(move_init)
{
    for(int i = 0; i < TEST_ITERATIONS; ++i)
    {
        // Initialize initial elements
        int num_elem = get_rand_int(1, 15);
        int *init_elems = new int[num_elem];

        for(int j = 0; j < num_elem; ++j)
        {
            init_elems[j] = get_rand_int(-100, 100);
        }

        // Init source Array
        Tundra_DynamicArrayint src;
        Tundra_DynArrint_init_w_elems(&src, init_elems, num_elem);

        uint64 saved_src_cap = src.cap;
        uint64 saved_src_cap_bytes = src.cap_bytes;
        uint64 saved_src_num_elem = src.num_elem;
        int *saved_src_data = src.data;

        // Init dest Array
        Tundra_DynamicArrayint dest;
        Tundra_DynArrint_init_w_move(&src, &dest);

        // Test correctness
        assert(dest.cap == saved_src_cap);
        assert(dest.cap_bytes == saved_src_cap_bytes);
        assert(dest.num_elem == saved_src_num_elem);
        assert(dest.data == saved_src_data);

        assert(src.cap == 0);
        assert(src.cap_bytes == 0);
        assert(src.num_elem == 0);
        assert(src.data == NULL);

        // Check each element. Dest data should be the same as init elements
        // since this is what the source contained.
        for(int j = 0; j < num_elem; j++)
        {
            assert(dest.data[j] == init_elems[j]);
        }

        Tundra_DynArrint_free(&src);
        Tundra_DynArrint_free(&dest);

        delete[] init_elems;
    }
}
TEST_END

TEST_BEGIN(free)
{
    Tundra_DynamicArrayint arr;
    Tundra_DynArrint_init(&arr);

    Tundra_DynArrint_free(&arr);

    assert(arr.cap == 0);
    assert(arr.cap_bytes == 0);
    assert(arr.num_elem == 0);
    assert(arr.data == NULL);
}
TEST_END

TEST_BEGIN(copy)
{
    for(int i = 0; i < TEST_ITERATIONS; ++i)
    {
        // Initialize initial elements
        int num_elem = get_rand_int(1, 15);
        int *init_elems = new int[num_elem];

        for(int j = 0; j < num_elem; ++j)
        {
            init_elems[j] = get_rand_int(-100, 100);
        }

        // Init source Array
        Tundra_DynamicArrayint src;
        Tundra_DynArrint_init_w_elems(&src, init_elems, num_elem);

        // Init dest Array.
        Tundra_DynamicArrayint dest;
        Tundra_DynArrint_init(&dest);

        Tundra_DynArrint_copy(&src, &dest);

        // Test correctness
        assert(dest.cap == src.cap);
        assert(dest.cap_bytes == src.cap_bytes);
        assert(dest.num_elem == src.num_elem);
        assert(dest.data && dest.data != src.data);

        // Check each element
        for(int j = 0; j < num_elem; j++)
        {
            assert(dest.data[j] == src.data[j]);
        }

        Tundra_DynArrint_free(&src);
        Tundra_DynArrint_free(&dest);

        delete[] init_elems;
    }
}
TEST_END

TEST_BEGIN(move)
{
    for(int i = 0; i < TEST_ITERATIONS; ++i)
    {
        // Initialize initial elements
        int num_elem = get_rand_int(1, 15);
        int *init_elems = new int[num_elem];

        for(int j = 0; j < num_elem; ++j)
        {
            init_elems[j] = get_rand_int(-100, 100);
        }

        // Init source Array
        Tundra_DynamicArrayint src;
        Tundra_DynArrint_init_w_elems(&src, init_elems, num_elem);

        uint64 saved_src_cap = src.cap;
        uint64 saved_src_cap_bytes = src.cap_bytes;
        uint64 saved_src_num_elem = src.num_elem;
        int *saved_src_data = src.data;

        // Init dest Array
        Tundra_DynamicArrayint dest;
        Tundra_DynArrint_init(&dest);

        Tundra_DynArrint_move(&src, &dest);

        // Test correctness
        assert(dest.cap == saved_src_cap);
        assert(dest.cap_bytes == saved_src_cap_bytes);
        assert(dest.num_elem == saved_src_num_elem);
        assert(dest.data == saved_src_data);

        assert(src.cap == 0);
        assert(src.cap_bytes == 0);
        assert(src.num_elem == 0);
        assert(src.data == NULL);

        // Check each element. Dest data should be the same as init elements
        // since this is what the source contained.
        for(int j = 0; j < num_elem; j++)
        {
            assert(dest.data[j] == init_elems[j]);
        }

        Tundra_DynArrint_free(&src);
        Tundra_DynArrint_free(&dest);

        delete[] init_elems;
    }
}
TEST_END

TEST_BEGIN(clear)
{
    for(int i = 0; i < TEST_ITERATIONS; ++i)
    {
        // Init initial elements
        int num_elem = get_rand_int(1, 15);
        int *init_elems = new int[num_elem];

        for(int j = 0; j < num_elem; ++j)
        {
            init_elems[j] = get_rand_int(-100, 100);
        }

        // Init Array
        Tundra_DynamicArrayint arr;
        Tundra_DynArrint_init_w_elems(&arr, init_elems, num_elem);

        uint64 saved_cap = arr.cap;
        uint64 saved_cap_bytes = arr.cap_bytes;
        int *saved_data = arr.data;

        Tundra_DynArrint_clear(&arr);

        assert(arr.cap == saved_cap);
        assert(arr.cap_bytes == saved_cap_bytes);
        assert(arr.num_elem == 0);
        assert(arr.data == saved_data);

        Tundra_DynArrint_free(&arr);

        delete[] init_elems;
    }
}
TEST_END

TEST_BEGIN(add)
{
    for(int i = 0; i < TEST_ITERATIONS; ++i)
    {
        // Init initial elements.
        int num_elem = get_rand_int(TUNDRA_DYNARR_DEF_CAP, 15);
        int *init_elems = new int[num_elem];

        for(int j = 0; j < num_elem; ++j)
        {
            init_elems[j] = get_rand_int(-100, 100);
        }

        // Init Array
        Tundra_DynamicArrayint arr;
        Tundra_DynArrint_init(&arr);

        // Add in each initial element.
        for(int j = 0; j < num_elem; ++j)
        {
            Tundra_DynArrint_add(&arr, init_elems + j);
        }

        // Test correctness
        int expected_cap_bytes = CALC_CAP_BYTES(num_elem);

        assert(arr.cap == expected_cap_bytes / sizeof(int));
        assert(arr.cap_bytes == expected_cap_bytes);
        assert(arr.num_elem == num_elem);
        assert(arr.data);

        // Check each element.
        for(int j = 0; j < num_elem; ++j)
        {
            assert(arr.data[j] == init_elems[j]);
        }

        Tundra_DynArrint_free(&arr);

        delete[] init_elems;
    }

}
TEST_END

TEST_BEGIN(add_multiple)
{
    for(int i = 0; i < TEST_ITERATIONS; ++i)
    {
        
    }
}
TEST_END

TEST_MAIN(DynArrTest)

