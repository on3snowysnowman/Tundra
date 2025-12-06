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
    Tundra_Stack5int stk = TUNDRA_MAKE_STK(int, 1, 5, 4, 2, 3);

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

    stk.num_elements = 3;

    Tundra_Stk5int_clear(&stk);

    assert(stk.num_elements == 0);
}
TEST_END

TEST_BEGIN(push)
{
    Tundra_Stack5int stk;
    Tundra_Stk5int_init(&stk);

    assert(Tundra_Stk5int_push(&stk, 10) == true);
    assert(Tundra_Stk5int_push(&stk, 20) == true);
    assert(Tundra_Stk5int_push(&stk, 30) == true);
    assert(Tundra_Stk5int_push(&stk, 40) == true);
    assert(Tundra_Stk5int_push(&stk, 50) == true);
    assert(Tundra_Stk5int_push(&stk, 60) == false); // Stack is full

    assert(stk.num_elements == 5);
    assert(stk.data[0] == 10);
    assert(stk.data[1] == 20);
    assert(stk.data[2] == 30);
    assert(stk.data[3] == 40);
    assert(stk.data[4] == 50);
}
TEST_END

TEST_BEGIN(pop)
{
    Tundra_Stack5int stk = TUNDRA_MAKE_STK(int, 100, 200, 300);

    Tundra_Stk5int_pop(&stk);
    assert(stk.num_elements == 2);
    assert(stk.data[0] == 100);
    assert(stk.data[1] == 200);

    Tundra_Stk5int_pop(&stk);
    assert(stk.num_elements == 1);
    assert(stk.data[0] == 100);

    Tundra_Stk5int_pop(&stk);
    assert(stk.num_elements == 0);
}
TEST_END

TEST_BEGIN(is_empty)
{
    Tundra_Stack5int stk;
    Tundra_Stk5int_init(&stk);

    assert(Tundra_Stk5int_is_empty(&stk) == true);

    Tundra_Stk5int_push(&stk, 42);
    assert(Tundra_Stk5int_is_empty(&stk) == false);
}
TEST_END

TEST_BEGIN(is_full)
{
    Tundra_Stack5int stk;
    Tundra_Stk5int_init(&stk);

    assert(Tundra_Stk5int_is_full(&stk) == false);

    Tundra_Stk5int_push(&stk, 1);
    Tundra_Stk5int_push(&stk, 2);
    Tundra_Stk5int_push(&stk, 3);
    Tundra_Stk5int_push(&stk, 4);
    Tundra_Stk5int_push(&stk, 5);

    assert(Tundra_Stk5int_is_full(&stk) == true);
}
TEST_END

TEST_BEGIN(front)
{
    Tundra_Stack5int stk = TUNDRA_MAKE_STK(int, 7, 14, 21);

    int *top = Tundra_Stk5int_front(&stk);
    assert(*top == 7);

    // Modify the top element
    *top = 42;
    assert(stk.data[0] == 42);
    assert(*Tundra_Stk5int_front(&stk) == 42);
    
    // Const Tests
    const Tundra_Stack5int const_stk = stk;
    const int *const_top = Tundra_Stk5int_front_cst(&const_stk);
    assert(*const_top == 42);
}
TEST_END

TEST_BEGIN(size)
{
    Tundra_Stack5int stk;
    Tundra_Stk5int_init(&stk);

    assert(Tundra_Stk5int_size(&stk) == 0);

    Tundra_Stk5int_push(&stk, 11);
    Tundra_Stk5int_push(&stk, 22);

    assert(Tundra_Stk5int_size(&stk) == 2);

    Tundra_Stk5int_clear(&stk);
    assert(Tundra_Stk5int_size(&stk) == 0);
}
TEST_END

int main()
{
    std::cout << "StackTest: \n";

    assert(Tundra_init() == 0);

    run_all_tests();

    std::cout << "\n";
    return 0;
}
