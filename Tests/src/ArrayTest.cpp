/**
 * @file ArrayTest.cpp
 * @author Joel Height (On3SnowySNowman@gmail.com)
 * @brief Test file for testing the Array container.
 * @version 0.1
 * @date 2025-12-30
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include <iostream>
#include <cassert>

#include "../TestingHelper.hpp"
#include "tundra/containers/Array4int.h"


TEST_BEGIN(make)
{
    Tundra_Array4int arr = TUNDRA_MAKE_ARRAY(1, 2, 3, 4);

    assert(arr.data[0] == 1);
    assert(arr.data[1] == 2);
    assert(arr.data[2] == 3);
    assert(arr.data[3] == 4);
}
TEST_END

TEST_BEGIN(at)
{
    Tundra_Array4int arr = TUNDRA_MAKE_ARRAY(1, 2, 3, 4);

    assert(*Tundra_Arr4int_at(&arr, 0) == 1);
    assert(*Tundra_Arr4int_at(&arr, 1) == 2);
    assert(*Tundra_Arr4int_at(&arr, 2) == 3);
    assert(*Tundra_Arr4int_at(&arr, 3) == 4);

    assert(*Tundra_Arr4int_at_cst(&arr, 0) == 1);
    assert(*Tundra_Arr4int_at_cst(&arr, 1) == 2);
    assert(*Tundra_Arr4int_at_cst(&arr, 2) == 3);
    assert(*Tundra_Arr4int_at_cst(&arr, 3) == 4);

    assert(*Tundra_Arr4int_at_nocheck(&arr, 0) == 1);
    assert(*Tundra_Arr4int_at_nocheck(&arr, 1) == 2);
    assert(*Tundra_Arr4int_at_nocheck(&arr, 2) == 3);
    assert(*Tundra_Arr4int_at_nocheck(&arr, 3) == 4);

    assert(*Tundra_Arr4int_at_nocheck_cst(&arr, 0) == 1);
    assert(*Tundra_Arr4int_at_nocheck_cst(&arr, 1) == 2);
    assert(*Tundra_Arr4int_at_nocheck_cst(&arr, 2) == 3);
    assert(*Tundra_Arr4int_at_nocheck_cst(&arr, 3) == 4);
}
TEST_END

TEST_BEGIN(front_back)
{
    Tundra_Array4int arr = TUNDRA_MAKE_ARRAY(1, 2, 3, 4);

    assert(*Tundra_Arr4int_front(&arr) == 1);
    assert(*Tundra_Arr4int_front_cst(&arr) == 1);
    assert(*Tundra_Arr4int_back(&arr) == 4);
    assert(*Tundra_Arr4int_back_cst(&arr) == 4);
}
TEST_END

TEST_BEGIN(size)
{
    assert(Tundra_Arr4int_size() == 4);
}
TEST_END


TEST_MAIN(ArrayTest)
