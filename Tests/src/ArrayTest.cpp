/**
 * @file ArrayTest.cpp
 * @author Joel Height (On3SnowySnowman)
 * @brief Test file for testing the Array container.
 * @date 2025-11-17
 * 
 * @copyright Copyright (c) 2025
*/

#include <iostream>
#include <cassert>

#define TUNDRA_TYPE int
#define TUNDRA_CAP 5
#include "tundra/containers/Array.h"
#undef TUNDRA_TYPE
#undef TUNDRA_CAP

#include "tundra/Tundra.h"

#include "../TestingHelper.hpp"

TEST_BEGIN(make)
{
    Tundra_Array5int arr = TUNDRA_MAKE_ARR(1, 5, 4, 2, 3);

    assert(arr.data[0] == 1);
    assert(arr.data[1] == 5);
    assert(arr.data[2] == 4);
    assert(arr.data[3] == 2);
    assert(arr.data[4] == 3);
}
TEST_END

TEST_BEGIN(at)
{
    Tundra_Array5int arr = TUNDRA_MAKE_ARR(1, 5, 4, 2, 3);

    assert(*Tundra_Arr5int_at_nochk(&arr, 0) == 1);
    assert(*Tundra_Arr5int_at_nochk(&arr, 1) == 5);
    assert(*Tundra_Arr5int_at_nochk(&arr, 2) == 4);
    assert(*Tundra_Arr5int_at_nochk(&arr, 3) == 2);
    assert(*Tundra_Arr5int_at_nochk(&arr, 4) == 3);
    assert(*Tundra_Arr5int_at(&arr, 0) == 1);
    assert(*Tundra_Arr5int_at(&arr, 1) == 5);
    assert(*Tundra_Arr5int_at(&arr, 2) == 4);
    assert(*Tundra_Arr5int_at(&arr, 3) == 2);
    assert(*Tundra_Arr5int_at(&arr, 4) == 3);

    // Const tests

    const Tundra_Array5int const_arr = 
        TUNDRA_MAKE_ARR(1, 5, 4, 2, 3);

    assert(*Tundra_Arr5int_at_nochk_cst(&const_arr, 0) == 1);
    assert(*Tundra_Arr5int_at_nochk_cst(&const_arr, 1) == 5);
    assert(*Tundra_Arr5int_at_nochk_cst(&const_arr, 2) == 4);
    assert(*Tundra_Arr5int_at_nochk_cst(&const_arr, 3) == 2);
    assert(*Tundra_Arr5int_at_nochk_cst(&const_arr, 4) == 3);
    assert(*Tundra_Arr5int_at_cst(&const_arr, 0) == 1);
    assert(*Tundra_Arr5int_at_cst(&const_arr, 1) == 5);
    assert(*Tundra_Arr5int_at_cst(&const_arr, 2) == 4);
    assert(*Tundra_Arr5int_at_cst(&const_arr, 3) == 2);
    assert(*Tundra_Arr5int_at_cst(&const_arr, 4) == 3);
}
TEST_END

TEST_BEGIN(front_back)
{
    Tundra_Array5int arr = TUNDRA_MAKE_ARR(1, 5, 4, 2, 3);

    assert(*Tundra_Arr5int_front(&arr) == 1);
    assert(*Tundra_Arr5int_back(&arr) == 3);

    // Const tests

    const Tundra_Array5int const_arr = 
        TUNDRA_MAKE_ARR(1, 5, 4, 2, 3);

    assert(*Tundra_Arr5int_front_cst(&const_arr) == 1);
    assert(*Tundra_Arr5int_back_cst(&const_arr) == 3);
}
TEST_END

TEST_BEGIN(size)
{
    Tundra_Array5int arr = TUNDRA_MAKE_ARR(1, 5, 4, 2, 3);

    assert(Tundra_Arr5int_size() == 5);

    // Const tests

    const Tundra_Array5int const_arr = 
        TUNDRA_MAKE_ARR(1, 5, 4, 2, 3);

    assert(Tundra_Arr5int_size() == 5);
}
TEST_END


int main()
{
    if(Tundra_init() != 0)
    {
        std::cerr << "Failed to initialize Tundra library!\n";
        return -1;
    }

    run_all_tests();
    return 0;
}