/**
 * @file Stack.cpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Test file for testing the Stack container.
 * @version 0.1
 * @date 2025-12-31
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include <iostream>
#include <cassert>

#include "../TestingHelper.hpp"
#include "tundra/containers/Stack4int.h"

TEST_BEGIN(init)
{
    Tundra_Stack4int stk;
    Tundra_Stk4int_init(&stk);

    assert(stk.num_elem == 0);
}
TEST_END

TEST_BEGIN(clear)
{
    Tundra_Stack4int stk;
    Tundra_Stk4int_init(&stk);

    Tundra_Stk4int_clear(&stk);

    assert(stk.num_elem == 0);
}
TEST_END

TEST_BEGIN(push)
{
    Tundra_Stack4int stk;
    Tundra_Stk4int_init(&stk);

    assert(Tundra_Stk4int_push(&stk, (int[]){2}));

    assert(stk.num_elem == 1);
    assert(stk.data[0] == 2);

    assert(Tundra_Stk4int_push(&stk, (int[]){5}));

    assert(stk.num_elem == 2);
    assert(stk.data[1] == 5);

    (Tundra_Stk4int_push(&stk, (int[]){1}));
    (Tundra_Stk4int_push(&stk, (int[]){4}));

    assert(Tundra_Stk4int_push(&stk, (int[]){1}) == false);
}
TEST_END

TEST_BEGIN(pop)
{
    Tundra_Stack4int stk;
    Tundra_Stk4int_init(&stk);

    Tundra_Stk4int_push(&stk, (int[]){3});
    Tundra_Stk4int_push(&stk, (int[]){9});

    Tundra_Stk4int_pop(&stk);

    assert(stk.data[stk.num_elem - 1] == 3);
}
TEST_END

TEST_BEGIN(is_empty)
{
    Tundra_Stack4int stk;
    Tundra_Stk4int_init(&stk);

    assert(Tundra_Stk4int_is_empty(&stk));
}
TEST_END

TEST_BEGIN(is_full)
{
    Tundra_Stack4int stk;
    Tundra_Stk4int_init(&stk);

    Tundra_Stk4int_push(&stk, (int[]){1});
    Tundra_Stk4int_push(&stk, (int[]){2});
    Tundra_Stk4int_push(&stk, (int[]){3});
    Tundra_Stk4int_push(&stk, (int[]){4});

    assert(Tundra_Stk4int_is_full(&stk));
}
TEST_END

TEST_BEGIN(front)
{
    Tundra_Stack4int stk;
    Tundra_Stk4int_init(&stk);

    Tundra_Stk4int_push(&stk, (int[]){1});

    assert(*Tundra_Stk4int_front(&stk) == 1);
    assert(*Tundra_Stk4int_front_cst(&stk) == 1);

    Tundra_Stk4int_push(&stk, (int[]){2});

    assert(*Tundra_Stk4int_front(&stk) == 2);
    assert(*Tundra_Stk4int_front_cst(&stk) == 2);
}
TEST_END

TEST_BEGIN(size)
{
    Tundra_Stack4int stk;
    Tundra_Stk4int_init(&stk);

    Tundra_Stk4int_push(&stk, (int[]){1});
    Tundra_Stk4int_push(&stk, (int[]){2});

    assert(Tundra_Stk4int_size(&stk) == 2);
}
TEST_END

TEST_BEGIN(capacity)
{
    assert(Tundra_Stk4int_capacity() == 4);
}
TEST_END

TEST_MAIN(StackTest)
