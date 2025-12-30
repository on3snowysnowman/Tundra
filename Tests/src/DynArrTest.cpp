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
    Tundra_ceil_pow2((num_elem) * sizeof(int))

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
        // Init initial elements
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
        // Init initial elements
        int num_elem = get_rand_int(TUNDRA_DYNARR_DEF_CAP, 15);
        int *init_elems = new int[num_elem];

        for(int j = 0; j < num_elem; ++j)
        {
            init_elems[j] = get_rand_int(-100, 100);
        }

        // Init Array
        Tundra_DynamicArrayint arr;
        Tundra_DynArrint_init(&arr);

        // Add in each initial element
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
        // Init initial elements
        int num_elem = get_rand_int(TUNDRA_DYNARR_DEF_CAP, 15);
        int *init_elems = new int[num_elem];

        for(int j = 0; j < num_elem; ++j)
        {
            init_elems[j] = get_rand_int(-100, 100);
        }

        // Init Array
        Tundra_DynamicArrayint arr;
        Tundra_DynArrint_init(&arr);

        // Add in elements
        Tundra_DynArrint_add_multiple(&arr, init_elems, num_elem);

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

TEST_BEGIN(insert)
{
    for(int i = 0; i < TEST_ITERATIONS; ++i)
    {
        // Init initial elements
        int num_elem = get_rand_int(TUNDRA_DYNARR_DEF_CAP, 15);
        int *init_elems = new int[num_elem];

        for(int j = 0; j < num_elem; ++j)
        {
            init_elems[j] = get_rand_int(-100, 100);
        }
                
        int insert_index = get_rand_int(0, num_elem);
        int insert_val = get_rand_int(-100, 100);

        // Init Array
        Tundra_DynamicArrayint arr;
        Tundra_DynArrint_init_w_elems(&arr, init_elems, num_elem);

        Tundra_DynArrint_insert(&arr, &insert_val, insert_index);

        // Test correctness.
        int expected_cap_bytes = CALC_CAP_BYTES(num_elem + 1);

        assert(arr.cap == expected_cap_bytes / sizeof(int));
        assert(arr.cap_bytes == expected_cap_bytes);
        assert(arr.num_elem == num_elem + 1);
        assert(arr.data);

        // Check elements before insert index
        for(int j = 0; j < insert_index; ++j)
        {
            assert(arr.data[j] == init_elems[j]);
        }
        
        // Check insert index
        assert(arr.data[insert_index] == insert_val);

        // Check elements after insert index
        for(int j = insert_index + 1; j < num_elem - insert_index; ++j)
        {
            assert(arr.data[j] == init_elems[j - 1]);
        }

        Tundra_DynArrint_free(&arr);

        delete[] init_elems;
    }
}
TEST_END

TEST_BEGIN(resize_smaller)
{
    for(int i = 0; i < TEST_ITERATIONS; ++i)
    {
        // Init initial elements
        int num_elem = get_rand_int(5, 31);
        int *init_elems = new int[num_elem];

        for(int j = 0; j < num_elem; ++j)
        {
            init_elems[j] = get_rand_int(-100, 100);
        }

        // Init Array
        Tundra_DynamicArrayint arr;
        Tundra_DynArrint_init_w_elems(&arr, init_elems, num_elem);

        static constexpr int RESIZE_SIZE = 4;

        Tundra_DynArrint_resize(&arr, RESIZE_SIZE);

        // Test correctness
        int expected_cap_bytes = CALC_CAP_BYTES(num_elem);

        assert(arr.cap == expected_cap_bytes / sizeof(int));
        assert(arr.cap_bytes == expected_cap_bytes);
        assert(arr.num_elem == RESIZE_SIZE);
        assert(arr.data);

        // Check elements
        for(int j = 0; j < RESIZE_SIZE; ++j)
        {
            assert(arr.data[j] == init_elems[j]);
        }

        Tundra_DynArrint_free(&arr);

        delete[] init_elems;
    }
}
TEST_END

TEST_BEGIN(resize_larger)
{
    for(int i = 0; i < TEST_ITERATIONS; ++i)
    {
        // Init initial elements
        int num_elem = get_rand_int(TUNDRA_DYNARR_DEF_CAP, 8);
        int *init_elems = new int[num_elem];

        for(int j = 0; j < num_elem; ++j)
        {
            init_elems[j] = get_rand_int(-100, 100);
        }

        // Init Array
        Tundra_DynamicArrayint arr;
        Tundra_DynArrint_init_w_elems(&arr, init_elems, num_elem);

        static constexpr int RESIZE_SIZE = 9;

        Tundra_DynArrint_resize(&arr, RESIZE_SIZE);

        // Test correctness
        int expected_cap_bytes = CALC_CAP_BYTES(RESIZE_SIZE);

        assert(arr.cap == expected_cap_bytes / sizeof(int));
        assert(arr.cap_bytes == expected_cap_bytes);
        assert(arr.num_elem == RESIZE_SIZE);
        assert(arr.data);

        // Check elements up to expansion
        for(int j = 0; j < num_elem; ++j)
        {
            assert(arr.data[j] == init_elems[j]);
        }

        Tundra_DynArrint_free(&arr);

        delete[] init_elems;
    }
}
TEST_END

TEST_BEGIN(reserve)
{
    for(int i = 0; i < TEST_ITERATIONS; ++i)
    {
        // Init initial elements
        int num_elem = get_rand_int(TUNDRA_DYNARR_DEF_CAP, 16);
        int *init_elems = new int[num_elem];

        for(int j = 0; j < num_elem; ++j)
        {
            init_elems[j] = get_rand_int(-100, 100);
        }

        // Init Array
        Tundra_DynamicArrayint arr;
        Tundra_DynArrint_init_w_elems(&arr, init_elems, num_elem);

        static constexpr int NUM_RESERVE = 3;

        Tundra_DynArrint_reserve(&arr, NUM_RESERVE);

        // Test correctness
        int expected_cap_bytes = CALC_CAP_BYTES(num_elem + NUM_RESERVE);

        assert(arr.cap == expected_cap_bytes / sizeof(int));
        assert(arr.cap_bytes == expected_cap_bytes);
        assert(arr.num_elem == num_elem);
        assert(arr.data);

        Tundra_DynArrint_free(&arr);

        delete[] init_elems;
    }
}
TEST_END

TEST_BEGIN(shrink_to_cap)
{
    for(int i = 0; i < TEST_ITERATIONS; ++i)
    {
        // Init initial elements
        int num_elem = get_rand_int(9, 16);
        int *init_elems = new int[num_elem];

        for(int j = 0; j < num_elem; ++j)
        {
            init_elems[j] = get_rand_int(-100, 100);
        }

        // Init Array
        Tundra_DynamicArrayint arr;
        Tundra_DynArrint_init_w_elems(&arr, init_elems, num_elem);

        static constexpr int SHRINK_CAP = 5;

        Tundra_DynArrint_shrink_to_new_cap(&arr, SHRINK_CAP);

        // Test correctness
        int expected_cap_bytes = CALC_CAP_BYTES(SHRINK_CAP);

        assert(arr.cap == expected_cap_bytes / sizeof(int));
        assert(arr.cap_bytes == expected_cap_bytes);
        assert(arr.num_elem == SHRINK_CAP);
        assert(arr.data);

        // Check elements to the shrink
        for(int j = 0; j < SHRINK_CAP; ++j)
        {
            assert(arr.data[j] == init_elems[j]);
        }

        Tundra_DynArrint_free(&arr);

        delete[] init_elems;
    }
}
TEST_END

TEST_BEGIN(shrink_to_fit)
{
    Tundra_DynamicArrayint arr;
    Tundra_DynArrint_init_w_cap(&arr, 18);

    assert(arr.cap == 32);

    Tundra_DynArrint_add(&arr, (int[]){1});
    Tundra_DynArrint_add(&arr, (int[]){1});
    Tundra_DynArrint_add(&arr, (int[]){1});
    Tundra_DynArrint_add(&arr, (int[]){1});
    Tundra_DynArrint_add(&arr, (int[]){1});

    Tundra_DynArrint_shrink_to_fit(&arr);

    assert(arr.cap == 8);

    Tundra_DynArrint_free(&arr);
}
TEST_END

TEST_BEGIN(erase)
{
    for(int i = 0; i < TEST_ITERATIONS; ++i)
    {
        // Init initial elements
        int num_elem = get_rand_int(1, 17);
        int *init_elems = new int[num_elem];

        for(int j = 0; j < num_elem; ++j)
        {
            init_elems[j] = get_rand_int(-100, 100);
        }

        // Init Array
        Tundra_DynamicArrayint arr;
        Tundra_DynArrint_init_w_elems(&arr, init_elems, num_elem);

        int erase_index = get_rand_int(0, num_elem - 1);

        Tundra_DynArrint_erase(&arr, erase_index);

        // Test correctness
        assert(arr.num_elem == num_elem - 1);
        assert(arr.data);

        // Check elements before erase index
        for(int j = 0; j < erase_index; ++j)
        {
            assert(arr.data[j] == init_elems[j]);
        }

        // Check elements at and after erase index
        for(int j = erase_index; j < num_elem - 1; ++j)
        {
            assert(arr.data[j] == init_elems[j + 1]);
        }

        Tundra_DynArrint_free(&arr);

        delete[] init_elems;
    }
}
TEST_END

TEST_BEGIN(erase_back)
{
    // Init initial elements
    int num_elem = get_rand_int(1, 17);
    int *init_elems = new int[num_elem];

    for(int j = 0; j < num_elem; ++j)
    {
        init_elems[j] = get_rand_int(-100, 100);
    }

    // Init Array
    Tundra_DynamicArrayint arr;
    Tundra_DynArrint_init_w_elems(&arr, init_elems, num_elem);

    Tundra_DynArrint_erase_back(&arr);

    assert(arr.num_elem == num_elem - 1);
    assert(arr.data);

    // Check each element up to the last one, which was erased.
    for(int j = 0; j < num_elem - 1; ++j)
    {
        assert(arr.data[j] == init_elems[j]);
    }

    Tundra_DynArrint_free(&arr);

    delete[] init_elems;
}
TEST_END

TEST_BEGIN(swap_and_pop)
{
    for(int i = 0; i < TEST_ITERATIONS; ++i)
    {
        // Init initial elements
        int num_elem = get_rand_int(1, 17);
        int *init_elems = new int[num_elem];

        for(int j = 0; j < num_elem; ++j)
        {
            init_elems[j] = get_rand_int(-100, 100);
        }

        // Init Array
        Tundra_DynamicArrayint arr;
        Tundra_DynArrint_init_w_elems(&arr, init_elems, num_elem);

        int erase_index = get_rand_int(0, num_elem - 1);

        Tundra_DynArrint_swap_and_pop(&arr, erase_index);

        // Test correctness
        assert(arr.num_elem == num_elem - 1);
        assert(arr.data);

        // Check elements before erase index
        for(int j = 0; j < erase_index; ++j)
        {
            assert(arr.data[j] == init_elems[j]);
        }

        // Ensure erase index is the last element.
        assert(arr.data[erase_index] == init_elems[num_elem - 1]);

        // Check elements at and after erase index
        for(int j = erase_index + 1; j < num_elem - 1; ++j)
        {
            assert(arr.data[j] == init_elems[j]);
        }

        Tundra_DynArrint_free(&arr);

        delete[] init_elems;
    }
}
TEST_END

TEST_BEGIN(front_back)
{
    for(int i = 0; i < TEST_ITERATIONS; ++i)
    {
        // Init initial elements
        int num_elem = get_rand_int(1, 17);
        int *init_elems = new int[num_elem];

        for(int j = 0; j < num_elem; ++j)
        {
            init_elems[j] = get_rand_int(-100, 100);
        }

        // Init Array
        Tundra_DynamicArrayint arr;
        Tundra_DynArrint_init_w_elems(&arr, init_elems, num_elem);  

        assert(*Tundra_DynArrint_front(&arr) == init_elems[0]);
        assert(*Tundra_DynArrint_front_cst(&arr) == init_elems[0]);

        assert(*Tundra_DynArrint_back(&arr) == init_elems[num_elem - 1]);
        assert(*Tundra_DynArrint_back_cst(&arr) == init_elems[num_elem - 1]);

        Tundra_DynArrint_free(&arr);

        delete[] init_elems;
    }
}
TEST_END

TEST_BEGIN(at)
{
    for(int i = 0; i < TEST_ITERATIONS; ++i)
    {

        // Init initial elements
        int num_elem = get_rand_int(1, 17);
        int *init_elems = new int[num_elem];

        for(int j = 0; j < num_elem; ++j)
        {
            init_elems[j] = get_rand_int(-100, 100);
        }

        // Init Array
        Tundra_DynamicArrayint arr;
        Tundra_DynArrint_init_w_elems(&arr, init_elems, num_elem);  

        int check_index = get_rand_int(0, num_elem - 1);

        assert(*Tundra_DynArrint_at(&arr, check_index) == init_elems[check_index]);
        assert(*Tundra_DynArrint_at_cst(&arr, check_index) == 
            init_elems[check_index]);
        assert(*Tundra_DynArrint_at_nocheck(&arr, check_index) == 
            init_elems[check_index]);
        assert(*Tundra_DynArrint_at_nocheck_cst(&arr, check_index) == 
            init_elems[check_index]);

        Tundra_DynArrint_free(&arr);

        delete[] init_elems;
    }
}
TEST_END

TEST_BEGIN(size_cap)
{
    for(int i = 0; i < TEST_ITERATIONS; ++i)
    {
        // Init initial elements
        int num_elem = get_rand_int(1, 17);
        int *init_elems = new int[num_elem];

        for(int j = 0; j < num_elem; ++j)
        {
            init_elems[j] = get_rand_int(-100, 100);
        }

        // Init Array
        Tundra_DynamicArrayint arr;
        Tundra_DynArrint_init_w_elems(&arr, init_elems, num_elem);  

        assert(Tundra_DynArrint_size(&arr) == num_elem);
        assert(Tundra_DynArrint_capacity(&arr) == arr.cap);

        Tundra_DynArrint_free(&arr);

        delete[] init_elems;
    }
}
TEST_END
    
TEST_BEGIN(iterator)
{
     for(int i = 0; i < TEST_ITERATIONS; ++i)
    {
        // Init initial elements
        int num_elem = get_rand_int(1, 17);
        int *init_elems = new int[num_elem];

        for(int j = 0; j < num_elem; ++j)
        {
            init_elems[j] = get_rand_int(-100, 100);
        }

        // Init Array
        Tundra_DynamicArrayint arr;
        Tundra_DynArrint_init_w_elems(&arr, init_elems, num_elem);  

        Tundra_DynamicArrayIteratorint begin_it, end_it;

        begin_it = Tundra_DynArrIterint_begin(&arr);

        assert(begin_it.array == &arr);
        assert(begin_it.index == 0);

        end_it = Tundra_DynArrIterint_end(&arr);
        assert(end_it.array == &arr);
        assert(end_it.index == num_elem);

        for(int j = 0; j < num_elem; ++j)
        {
            assert(*Tundra_DynArrIterint_deref(&begin_it) == init_elems[j]);
            assert(*Tundra_DynArrIterint_deref_cst(&begin_it) == init_elems[j]);

            Tundra_DynArrIterint_next(&begin_it);
        }

        assert(Tundra_DynArrIterint_compare(&begin_it, &end_it));

        Tundra_DynArrint_free(&arr);

        delete[] init_elems;
    }
}
TEST_END

TEST_MAIN(DynArrTest)

