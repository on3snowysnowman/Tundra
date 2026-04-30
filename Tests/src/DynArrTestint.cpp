// DynArrTest.cpp

#include <iostream>
#include <random>
#include <cassert>

#include "../TestingHelper.hpp"
#include "tundra/utils/Math.h"
#include "tundra/containers/DynamicArrayint.h"


#define CALC_CAP_BYTES(num_elem) \
    Tundra_ceil_pow2((num_elem) * sizeof(int))

static constexpr int TEST_ITERATIONS = 50;


int get_rand_int(int min, int max)
{
    static std::mt19937 rng{ std::random_device{}() };
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

void init_arr_with_ran_elems(Tundra_DynamicArrayint *arr)
{
    const int num_elem = get_rand_int(1, 100);
    int * const init_elems = new int[num_elem];

    for(int j = 0; j < num_elem; ++j)
    {
        init_elems[j] = get_rand_int(-100, 100);
    }

    Tundra_DynArrint_init_w_elems(arr, init_elems, num_elem);

    delete[] init_elems;
}


TEST_BEGIN(basic_init)
{
    Tundra_DynamicArrayint arr;
    Tundra_DynArrint_init(&arr);

    assert(arr.cap == TUNDRA_DYNARR_DEF_CAP);
    assert(arr.cap_bytes == CALC_CAP_BYTES(arr.cap));
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

        const uint64_t expected_cap_bytes = CALC_CAP_BYTES(capacity);

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
        const int num_elem = get_rand_int(1, 100);
        int * const init_elems = new int[num_elem];

        for(int j = 0; j < num_elem; ++j)
        {
            init_elems[j] = get_rand_int(-100, 100);
        }

        Tundra_DynamicArrayint arr;
        Tundra_DynArrint_init_w_elems(&arr, init_elems, num_elem);

        uint64_t expected_cap_bytes = CALC_CAP_BYTES(num_elem);

        assert(arr.cap == expected_cap_bytes / sizeof(int));
        assert(arr.cap_bytes == expected_cap_bytes);
        assert(arr.num_elem == (uint64_t)num_elem);
        assert(arr.data);

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
        Tundra_DynamicArrayint src;
        init_arr_with_ran_elems(&src);

        Tundra_DynamicArrayint dst;
        Tundra_DynArrint_init_w_copy(&dst, &src);

        assert(dst.cap == src.cap);
        assert(dst.cap_bytes == src.cap_bytes);
        assert(dst.num_elem == src.num_elem);
        assert(dst.data && dst.data != src.data);

        for(int j = 0; j < (int)src.num_elem; ++j)
        {
            assert(dst.data[j] == src.data[j]);
        }

        Tundra_DynArrint_free(&src);
        Tundra_DynArrint_free(&dst);
    }
}
TEST_END

TEST_BEGIN(move_init)
{
    for(int i = 0; i < TEST_ITERATIONS; ++i)
    {
        Tundra_DynamicArrayint src;
        init_arr_with_ran_elems(&src);

        uint64 saved_src_cap = src.cap;
        uint64 saved_src_cap_bytes = src.cap_bytes;
        uint64 saved_src_num_elem = src.num_elem;
        int *saved_src_data = src.data;

        Tundra_DynamicArrayint dst;
        Tundra_DynArrint_init_w_move(&dst, &src);

        assert(dst.cap == saved_src_cap);
        assert(dst.cap_bytes == saved_src_cap_bytes);
        assert(dst.num_elem == saved_src_num_elem);
        assert(dst.data == saved_src_data);

        assert(src.cap == 0);
        assert(src.cap_bytes == 0);
        assert(src.num_elem == 0);
        assert(src.data == NULL);

        Tundra_DynArrint_free(&dst);
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
        Tundra_DynamicArrayint src;
        init_arr_with_ran_elems(&src);

        Tundra_DynamicArrayint dst;
        Tundra_DynArrint_init(&dst);

        Tundra_DynArrint_assign_by_copy(&dst, &src);

        assert(dst.cap == src.cap);
        assert(dst.cap_bytes == src.cap_bytes);
        assert(dst.num_elem == src.num_elem);
        assert(dst.data && dst.data != src.data);

        for(int j = 0; j < (int)dst.num_elem; ++j)
        {
            assert(dst.data[j] == src.data[j]);
        }

        Tundra_DynArrint_free(&src);
        Tundra_DynArrint_free(&dst);
    }
}
TEST_END

TEST_BEGIN(move)
{
    for(int i = 0; i < TEST_ITERATIONS; ++i)
    {
        Tundra_DynamicArrayint src;
        init_arr_with_ran_elems(&src);

        uint64 saved_src_cap = src.cap;
        uint64 saved_src_cap_bytes = src.cap_bytes;
        uint64 saved_src_num_elem = src.num_elem;
        int *saved_src_data = src.data;

        Tundra_DynamicArrayint dst;
        Tundra_DynArrint_init(&dst);

        Tundra_DynArrint_assign_by_move(&dst, &src);

        assert(dst.cap == saved_src_cap);
        assert(dst.cap_bytes == saved_src_cap_bytes);
        assert(dst.num_elem == saved_src_num_elem);
        assert(dst.data == saved_src_data);

        assert(src.cap == 0);
        assert(src.cap_bytes == 0);
        assert(src.num_elem == 0);
        assert(src.data == NULL);

        Tundra_DynArrint_free(&src);
        Tundra_DynArrint_free(&dst);
    }
}
TEST_END

TEST_BEGIN(clear)
{
    for(int i = 0; i < TEST_ITERATIONS; ++i)
    {
        Tundra_DynamicArrayint arr;
        init_arr_with_ran_elems(&arr);

        uint64 saved_cap = arr.cap;
        uint64 saved_cap_bytes = arr.cap_bytes;
        int *saved_data = arr.data;

        Tundra_DynArrint_clear(&arr);

        assert(arr.cap == saved_cap);
        assert(arr.cap_bytes == saved_cap_bytes);
        assert(arr.num_elem == 0);
        assert(arr.data == saved_data);

        Tundra_DynArrint_free(&arr);
    }
}
TEST_END

TEST_BEGIN(add_by_copy)
{
    for(int i = 0; i < TEST_ITERATIONS; ++i)
    {
        Tundra_DynamicArrayint arr;
        Tundra_DynArrint_init(&arr);

        int num = get_rand_int(-100, 100);

        Tundra_DynArrint_add_by_copy(&arr, &num);

        assert(arr.data[0] == num);

        Tundra_DynArrint_free(&arr);
    }
}
TEST_END

TEST_BEGIN(add_by_move)
{
    for(int i = 0; i < TEST_ITERATIONS; ++i)
    {
        Tundra_DynamicArrayint arr;
        Tundra_DynArrint_init(&arr);

        int num = get_rand_int(-100, 100);

        Tundra_DynArrint_add_by_move(&arr, &num);

        assert(arr.data[0] == num);

        Tundra_DynArrint_free(&arr);
    }
}
TEST_END

TEST_BEGIN(add_by_val)
{
    for(int i = 0; i < TEST_ITERATIONS; ++i)
    {
        Tundra_DynamicArrayint arr;
        Tundra_DynArrint_init(&arr);

        int num = get_rand_int(-100, 100);

        Tundra_DynArrint_add_by_val(&arr, num);

        assert(arr.data[0] == num);

        Tundra_DynArrint_free(&arr);
    }
}
TEST_END

TEST_BEGIN(insert_by_copy)
{
    for(int i = 0; i < TEST_ITERATIONS; ++i)
    {
        Tundra_DynamicArrayint arr;
        init_arr_with_ran_elems(&arr);

        


        Tundra_DynArrint_free(&arr);
    }
}
TEST_END

TEST_MAIN(DynArrint_Test)