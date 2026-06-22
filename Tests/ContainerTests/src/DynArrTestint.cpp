// DynArrTest.cpp

#include <iostream>
#include <random>
#include <cassert>

#include "../TestingHelper.hpp"
#include "tundra/utils/Math.h"
#include "tundra/containers/DynamicArrayint.h"


#define CALC_CAP_BYTES(num_elem) \
    Tundra_ceil_pow2((num_elem) * sizeof(int))


static constexpr int TEST_ITERATIONS = 1'000;


int get_rand_int(int min, int max)
{
    static std::mt19937 rng{ std::random_device{}() };
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

struct ArrInitRet
{
    int num_elem = 0;
    int * init_elems = nullptr;

    ~ArrInitRet()
    {
        if(init_elems) delete[] init_elems;
    }
};

ArrInitRet init_arr_with_ran_elems(Tundra_DynamicArrayint *arr, 
    int min_elem_count = 1, int max_elem_count = 100, int min_val = -100,
    int max_val = 100)
{
    ArrInitRet ret;

    ret.num_elem = get_rand_int(min_elem_count, max_elem_count);
    ret.init_elems = new int[ret.num_elem];

    // const int num_elem = get_rand_int(min_elem_count, max_elem_count);
    // int * const init_elems = new int[num_elem];

    for(int j = 0; j < ret.num_elem; ++j)
    {
        ret.init_elems[j] = get_rand_int(min_val, max_val);
    }

    Tundra_DynArrint_init_elems(arr, ret.init_elems, ret.num_elem);

    // delete[] init_elems;
    return ret;
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
        // int capacity = get_rand_int(1, 33);
        int capacity = 17;

        Tundra_DynamicArrayint arr;
        Tundra_DynArrint_init_cap(&arr, capacity);

        const u64 expected_cap_bytes = CALC_CAP_BYTES(capacity);

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
        Tundra_DynArrint_init_elems(&arr, init_elems, num_elem);

        u64 expected_cap_bytes = CALC_CAP_BYTES(num_elem);

        assert(arr.cap == expected_cap_bytes / sizeof(int));
        assert(arr.cap_bytes == expected_cap_bytes);
        assert(arr.num_elem == (u64)num_elem);
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
        ArrInitRet init_ret = init_arr_with_ran_elems(&src);

        Tundra_DynamicArrayint dst;
        Tundra_DynArrint_init_copy(&dst, &src);

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

        u64 saved_src_cap = src.cap;
        u64 saved_src_cap_bytes = src.cap_bytes;
        u64 saved_src_num_elem = src.num_elem;
        int *saved_src_data = src.data;

        Tundra_DynamicArrayint dst;
        Tundra_DynArrint_init_move(&dst, &src);

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

        Tundra_DynArrint_assign_copy(&dst, &src);

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

        u64 saved_src_cap = src.cap;
        u64 saved_src_cap_bytes = src.cap_bytes;
        u64 saved_src_num_elem = src.num_elem;
        int *saved_src_data = src.data;

        Tundra_DynamicArrayint dst;
        Tundra_DynArrint_init(&dst);

        Tundra_DynArrint_assign_move(&dst, &src);

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

        u64 saved_cap = arr.cap;
        u64 saved_cap_bytes = arr.cap_bytes;
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

TEST_BEGIN(add_copy)
{
    for(int i = 0; i < TEST_ITERATIONS; ++i)
    {
        Tundra_DynamicArrayint arr;
        Tundra_DynArrint_init(&arr);

        int num = get_rand_int(-100, 100);

        Tundra_DynArrint_add_copy(&arr, &num);

        assert(arr.data[0] == num);

        Tundra_DynArrint_free(&arr);
    }
}
TEST_END

TEST_BEGIN(add_move)
{
    for(int i = 0; i < TEST_ITERATIONS; ++i)
    {
        Tundra_DynamicArrayint arr;
        Tundra_DynArrint_init(&arr);

        int num = get_rand_int(-100, 100);

        Tundra_DynArrint_add_move(&arr, &num);

        assert(arr.data[0] == num);

        Tundra_DynArrint_free(&arr);
    }
}
TEST_END

TEST_BEGIN(add_val)
{
    for(int i = 0; i < TEST_ITERATIONS; ++i)
    {
        Tundra_DynamicArrayint arr;
        Tundra_DynArrint_init(&arr);

        int num = get_rand_int(-100, 100);

        Tundra_DynArrint_add_val(&arr, num);

        assert(arr.data[0] == num);

        Tundra_DynArrint_free(&arr);
    }
}
TEST_END

TEST_BEGIN(add_uninit)
{
    for(int i = 0; i < TEST_ITERATIONS; ++i)
    {
        Tundra_DynamicArrayint arr;
        Tundra_DynArrint_init(&arr);

        assert(Tundra_DynArrint_add_uninit(&arr) == arr.data);
        assert(arr.num_elem == 1);

        Tundra_DynArrint_free(&arr);
    }
}
TEST_END

TEST_BEGIN(add_mult_copy)
{   
    for(int i = 0; i < TEST_ITERATIONS; ++i)
    {
        Tundra_DynamicArrayint arr;
        ArrInitRet ret = init_arr_with_ran_elems(&arr);

        const u64 INITIAL_ELEM_CT = arr.num_elem;

        int *saved_arr_data = new int[arr.num_elem];

        for(u64 i = 0; i < arr.num_elem; ++i)
        {
            saved_arr_data[i] = arr.data[i];
        }

        const int num_add = get_rand_int(2, 20);
        int *elem_to_add = new int[num_add];

        for(int i = 0; i < num_add; ++i)
        {
            elem_to_add[i] = get_rand_int(-100, 100);
        }

        Tundra_DynArrint_add_mult_copy(&arr, elem_to_add, num_add);

        u64 expected_cap_bytes = CALC_CAP_BYTES(arr.num_elem);

        assert(arr.data);
        assert(arr.num_elem == INITIAL_ELEM_CT + num_add);
        assert(arr.cap_bytes == expected_cap_bytes);
        assert(arr.cap == expected_cap_bytes / sizeof(int));

        for(u64 i = 0; i < INITIAL_ELEM_CT; ++i)
        {
            assert(arr.data[i] == saved_arr_data[i]);
        }

        for(int i = 0; i < num_add; ++i)
        {
            assert(arr.data[INITIAL_ELEM_CT + i] == elem_to_add[i]);
        }

        delete[] saved_arr_data;
        delete[] elem_to_add;
        Tundra_DynArrint_free(&arr);
    }
}
TEST_END

TEST_BEGIN(insert_copy)
{
    for(int i = 0; i < TEST_ITERATIONS; ++i)
    {
        Tundra_DynamicArrayint arr;
        init_arr_with_ran_elems(&arr);

        u64 insert_idx = get_rand_int(0, arr.num_elem);

        int num = get_rand_int(-100, 100);

        Tundra_DynArrint_insert_copy(&arr, insert_idx, &num);

        assert(arr.data[insert_idx] == num);

        Tundra_DynArrint_free(&arr);
    }
}
TEST_END

TEST_BEGIN(insert_move)
{
    for(int i = 0; i < TEST_ITERATIONS; ++i)
    {
        Tundra_DynamicArrayint arr;
        init_arr_with_ran_elems(&arr);

        u64 insert_idx = get_rand_int(0, arr.num_elem);

        int num = get_rand_int(-100, 100);

        Tundra_DynArrint_insert_move(&arr, insert_idx, &num);

        assert(arr.data[insert_idx] == num);

        Tundra_DynArrint_free(&arr);
    }
}
TEST_END

TEST_BEGIN(insert_val)
{
    for(int i = 0; i < TEST_ITERATIONS; ++i)
    {
        Tundra_DynamicArrayint arr;
        init_arr_with_ran_elems(&arr);

        u64 insert_idx = get_rand_int(0, arr.num_elem);

        int num = get_rand_int(-100, 100);

        Tundra_DynArrint_insert_val(&arr, insert_idx, num);

        assert(arr.data[insert_idx] == num);

        Tundra_DynArrint_free(&arr);
    }
}
TEST_END

TEST_BEGIN(insert_uninit)
{
    for(int i = 0; i < TEST_ITERATIONS; ++i)
    {
        Tundra_DynamicArrayint arr;
        init_arr_with_ran_elems(&arr);

        const u64 insert_idx = get_rand_int(0, arr.num_elem);

        const int insert_val = get_rand_int(-100, 100);

        *Tundra_DynArrint_insert_uninit(&arr, insert_idx) = insert_val;

        assert(arr.data[insert_idx] == insert_val);

        Tundra_DynArrint_free(&arr);
    }
}
TEST_END

TEST_BEGIN(resize_smaller)
{
    for(int i = 0; i < TEST_ITERATIONS; ++i)
    {
        Tundra_DynamicArrayint arr;
        init_arr_with_ran_elems(&arr, 8);

        static constexpr int RESIZE_SIZE = 7;
        u64 expected_cap_bytes = CALC_CAP_BYTES(arr.num_elem);
        
        Tundra_DynArrint_resize(&arr, RESIZE_SIZE);

        assert(arr.cap == expected_cap_bytes / sizeof(int));
        assert(arr.cap_bytes == expected_cap_bytes);
        assert(arr.num_elem == RESIZE_SIZE);
        assert(arr.data);

        Tundra_DynArrint_free(&arr);
    }
}
TEST_END

TEST_BEGIN(resize_larger)
{
    for(int i = 0; i < TEST_ITERATIONS; ++i)
    {
        Tundra_DynamicArrayint arr;
        init_arr_with_ran_elems(&arr, 1, 63);

        static constexpr int RESIZE_SIZE = 64;
        
        u64 expected_cap_bytes = CALC_CAP_BYTES(RESIZE_SIZE);

        Tundra_DynArrint_resize(&arr, RESIZE_SIZE);

        assert(arr.cap == expected_cap_bytes / sizeof(int));
        assert(arr.cap_bytes == expected_cap_bytes);
        assert(arr.num_elem == RESIZE_SIZE);
        assert(arr.data);

        Tundra_DynArrint_free(&arr);
    }
}
TEST_END

TEST_BEGIN(reserve)
{
    for(int i = 0; i < TEST_ITERATIONS; ++i)
    {
        Tundra_DynamicArrayint arr; 
        init_arr_with_ran_elems(&arr);

        const u64 init_num_elem = Tundra_DynArrint_size(&arr);

        const int num_reserve = get_rand_int(4, 100);

        Tundra_DynArrint_reserve(&arr, num_reserve);
    
        assert(Tundra_DynArrint_capacity(&arr) == 
            CALC_CAP_BYTES(init_num_elem + num_reserve) / sizeof(int));

        Tundra_DynArrint_free(&arr);
    }
}
TEST_END

TEST_BEGIN(shrink)
{
    // shrink to fit
    for(int i = 0; i < TEST_ITERATIONS; ++i)
    {
        Tundra_DynamicArrayint arr;
        init_arr_with_ran_elems(&arr);

        const u64 expected_cap = 
            CALC_CAP_BYTES(Tundra_DynArrint_size(&arr)) / sizeof(int);

        Tundra_DynArrint_shrink_fit(&arr);

        assert(Tundra_DynArrint_capacity(&arr) == expected_cap);

        Tundra_DynArrint_free(&arr);
    }

    // shrink to cap
    for(int i = 0; i < TEST_ITERATIONS; ++i)
    {
        Tundra_DynamicArrayint arr;
        init_arr_with_ran_elems(&arr, 4);

        const u64 rand_cap = Tundra_DynArrint_capacity(&arr) - 
            get_rand_int(1, Tundra_DynArrint_capacity(&arr) - 1);

        const u64 expected_cap = CALC_CAP_BYTES(rand_cap) / sizeof(int);

        Tundra_DynArrint_shrink_new_cap(&arr, rand_cap);

        assert(Tundra_DynArrint_capacity(&arr) == expected_cap);

        Tundra_DynArrint_free(&arr);
    }

}
TEST_END

TEST_BEGIN(erase)
{
    // erase
    for(int i = 0; i < TEST_ITERATIONS; ++i)
    {
        Tundra_DynamicArrayint arr;
        ArrInitRet init_ret = init_arr_with_ran_elems(&arr);

        const int erase_idx = get_rand_int(0, init_ret.num_elem - 1);

        Tundra_DynArrint_erase(&arr, erase_idx);

        assert(Tundra_DynArrint_size(&arr) == init_ret.num_elem - 1);
        assert(arr.data);

        for(int j = 0; j < erase_idx; ++j)
        {
            assert(arr.data[j] == init_ret.init_elems[j]);
        }

        for(int j = erase_idx; j < init_ret.num_elem - 1; ++j)
        {
            assert(arr.data[j] == init_ret.init_elems[j + 1]);
        }

        Tundra_DynArrint_free(&arr);
    }

    // erase_back
    for(int i = 0; i < TEST_ITERATIONS; ++i)
    {
        Tundra_DynamicArrayint arr;
        ArrInitRet init_ret = init_arr_with_ran_elems(&arr);

        Tundra_DynArrint_erase_back(&arr);

        assert(arr.num_elem == init_ret.num_elem - 1);
        assert(arr.data);

        for(int j = 0; j < init_ret.num_elem; ++j)
        {
            assert(arr.data[j] == init_ret.init_elems[j]);
        }

        Tundra_DynArrint_free(&arr);
    }
}
TEST_END

TEST_BEGIN(erase_multiple)
{
    for(int i = 0; i < TEST_ITERATIONS; ++i)
    {
        Tundra_DynamicArrayint arr;
        ArrInitRet init_ret = init_arr_with_ran_elems(&arr, 3);

        const int erase_idx = get_rand_int(1, init_ret.num_elem - 1);
        // const int erase_idx = 2;
        
        const int num_erase = get_rand_int(1, init_ret.num_elem - erase_idx);
        // const int num_erase = 1;

        Tundra_DynArrint_erase_multiple(&arr, erase_idx, num_erase);

        assert(arr.num_elem == init_ret.num_elem - num_erase);
        assert(arr.data);

        // std::cout << "Init Elements: ";

        // for(u64 j = 0; j < init_ret.num_elem; ++j)
        // {
        //     std::cout << init_ret.init_elems[j] << ' ';
        // }

        // std::cout << "\nErasing " << num_erase << " elements at idx " << erase_idx << '\n';

        // std::cout << "After erase: ";

        // for(u64 j = 0; j < arr.num_elem; ++j)
        // {
        //     std::cout << arr.data[j] << ' ';
        // }

        // std::cout << '\n';

        for(u64 j = 0; j < erase_idx; ++j)
        {
            assert(arr.data[j] == init_ret.init_elems[j]);
        }

        for(u64 j = erase_idx; j < arr.num_elem; ++j)
        {
            assert(arr.data[j] == init_ret.init_elems[j + num_erase]);
        }

        Tundra_DynArrint_free(&arr);
    }
}
TEST_END

TEST_BEGIN(swap_pop)
{
    for(int i = 0; i < TEST_ITERATIONS; ++i)
    {
        Tundra_DynamicArrayint arr;
        ArrInitRet init_ret = init_arr_with_ran_elems(&arr);

        const int erase_idx = get_rand_int(0, init_ret.num_elem - 1); 

        Tundra_DynArrint_swap_pop(&arr, erase_idx);

        assert(arr.num_elem == init_ret.num_elem - 1);
        assert(arr.data);

        for(int j = 0; j < erase_idx; ++j)
        {
            assert(arr.data[j] == init_ret.init_elems[j]);
        }

        assert(arr.data[erase_idx] == 
            init_ret.init_elems[init_ret.num_elem - 1]);

        for(int j = erase_idx + 1; j < init_ret.num_elem - 1; ++j)
        {
            assert(arr.data[j] == init_ret.init_elems[j]);
        }

        Tundra_DynArrint_free(&arr);
    }
}
TEST_END

TEST_BEGIN(front_back)
{
    for(int i = 0; i < TEST_ITERATIONS; ++i)
    {
        Tundra_DynamicArrayint arr;
        ArrInitRet init_ret = init_arr_with_ran_elems(&arr);

        assert(*Tundra_DynArrint_front(&arr) == init_ret.init_elems[0]);
        assert(*Tundra_DynArrint_front_mut(&arr) == init_ret.init_elems[0]);
        assert(*Tundra_DynArrint_back(&arr) == 
            init_ret.init_elems[init_ret.num_elem - 1]);
        assert(*Tundra_DynArrint_back_mut(&arr) == 
            init_ret.init_elems[init_ret.num_elem - 1]);

        Tundra_DynArrint_free(&arr);
    }
}
TEST_END

TEST_BEGIN(at)
{
    for(int i = 0; i < TEST_ITERATIONS; ++i)
    {
        Tundra_DynamicArrayint arr;
        ArrInitRet init_ret = init_arr_with_ran_elems(&arr);

        const int check_idx = get_rand_int(0, init_ret.num_elem - 1);

        assert(*Tundra_DynArrint_at(&arr, check_idx) == 
            init_ret.init_elems[check_idx]);
        assert(*Tundra_DynArrint_at_mut(&arr, check_idx) == 
            init_ret.init_elems[check_idx]);

        assert(*Tundra_DynArrint_at_nochk(&arr, check_idx) == 
            init_ret.init_elems[check_idx]);
        assert(*Tundra_DynArrint_at_mut_nochk(&arr, check_idx) == 
            init_ret.init_elems[check_idx]);

        Tundra_DynArrint_free(&arr);
    }
}
TEST_END

TEST_BEGIN(size_cap)
{
    for(int i = 0; i < TEST_ITERATIONS; ++i)
    {
        Tundra_DynamicArrayint arr;
        ArrInitRet init_ret = init_arr_with_ran_elems(&arr);

        const u64 size = Tundra_DynArrint_size(&arr);
        const u64 cap = Tundra_DynArrint_capacity(&arr);
        
        assert(size == init_ret.num_elem);
        assert(size == arr.num_elem);

        const u64 expected_cap_bytes = 
            CALC_CAP_BYTES(init_ret.num_elem);

        assert(arr.cap_bytes == expected_cap_bytes);
        assert(arr.cap == cap);
        assert(arr.cap == expected_cap_bytes / sizeof(int));

        Tundra_DynArrint_free(&arr);
    }
}
TEST_END

TEST_MAIN(DynArrint_Test)
