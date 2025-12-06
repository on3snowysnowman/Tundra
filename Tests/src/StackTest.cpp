/**
 * @file StackTest.cpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Test file for testing the Stack container.
 * @date 2025-12-05
 * 
 * @copyright Copyright (c) 2025 
*/

#include <iostream>
#include <cassert>

#define TUNDRA_TYPE int
#define TUNDRA_CAP 5
#include "tundra/containers/Stack.h"
#undef TUNDRA_TYPE
#undef TUNDRA_CAP

#include "tundra/Tundra.h"

#include "../TestingHelper.hpp"

TEST_BEGIN(make)
{
    Tundra_Stack5int stk = TUNDRA_MAKE_STK(1, 5, 4, 2, 3);

    assert(stk.data[0] == 1);
    assert(stk.data[1] == 5);
    assert(stk.data[2] == 4);
    assert(stk.data[3] == 2);
    assert(stk.data[4] == 3); 
}
TEST_END

TEST_BEGIN(init)
{
    Tundra_Stack5int stk;
    Tundra_Stk5int_init(&stk);

    assert(stk.num_elements == 0);
}
TEST_END

TEST_BEGIN(clear)
{
    Tundra_Stack5int stk;
    Tundra_Stk5int_init(&stk);
    Tundra_Stk5int_clear(&stk);

    assert(stk.num_elements == 0);
}
TEST_END

TEST_BEGIN()


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
