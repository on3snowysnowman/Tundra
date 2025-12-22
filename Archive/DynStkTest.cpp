/**
 * @file DynStkTest.cpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Test file for testing the DynamicStack container.
 * @date 2025-11-23
 * 
 * @copyright Copyright (c) 2025
*/

#include <cassert>

#include "../TestingHelper.hpp"
#define TUNDRA_TYPE int
#include "tundra/containers/DynamicStack.h"
#undef TUNDRA_TYPE
#include "tundra/Tundra.h"


TEST_BEGIN(init)
{
    // Default init method
    Tundra_DynamicStackint stk_one;
    Tundra_DynStkint_init(&stk_one);

    assert(stk_one.cap == TUNDRA_DYNSTK_DEF_CAP);
    assert(stk_one.num_elem == 0);
    assert(stk_one.data);

    // Specified capacity init method
    Tundra_DynamicStackint stk_two;
    Tundra_DynStkint_init_w_cap(&stk_two, 10);

    assert(stk_two.cap == 10);
    assert(stk_two.num_elem == 0);
    assert(stk_two.data);

    // Initial elements init method
    constexpr int NUM_INIT_ELEM = 5;
    constexpr int elems[NUM_INIT_ELEM] = {1, 2, 3, 4, 5};

    Tundra_DynamicStackint stk_thr;

    // Test without strict alloc
    Tundra_DynStkint_init_w_elems(&stk_thr, elems, NUM_INIT_ELEM, false);
    
    assert(stk_thr.cap == 8);
    assert(stk_thr.num_elem == NUM_INIT_ELEM);
    assert(stk_thr.data);

    for(int i = 0; i < NUM_INIT_ELEM; ++i)
    {
        assert(stk_thr.data[i] == elems[i]);
    }

    Tundra_DynStkint_free(&stk_thr);

    // Test with strict alloc
    Tundra_DynStkint_init_w_elems(&stk_thr, elems, NUM_INIT_ELEM, true);

    assert(stk_thr.cap == NUM_INIT_ELEM);
    assert(stk_thr.num_elem == NUM_INIT_ELEM);
    assert(stk_thr.data);

    for(int i = 0; i < NUM_INIT_ELEM; ++i)
    {
        assert(stk_thr.data[i] == elems[i]);
    }

    Tundra_DynStkint_free(&stk_one);
    Tundra_DynStkint_free(&stk_two);
    Tundra_DynStkint_free(&stk_thr);
}
TEST_END

TEST_BEGIN(free)
{
    Tundra_DynamicStackint stk;
    Tundra_DynStkint_init(&stk);
    Tundra_DynStkint_free(&stk);

    assert(stk.data == nullptr);
}
TEST_END

TEST_BEGIN(copy)
{
    Tundra_DynamicStackint src;
    Tundra_DynamicStackint dst;

    constexpr int NUM_INIT_ELEM = 5;
    constexpr int elems[NUM_INIT_ELEM] = {1, 2, 3, 4, 5};

    Tundra_DynStkint_init_w_elems(&src, elems, NUM_INIT_ELEM, false);

    // dst purposely left uninitialized.
    Tundra_DynStkint_copy(&src, &dst);

    assert(dst.cap == src.cap);
    assert(dst.num_elem == src.num_elem);
    assert(dst.data != src.data);

    for(int i = 0; i < NUM_INIT_ELEM; ++i)
    {
        assert(dst.data[i] == elems[i]);
    }

    Tundra_DynStkint_free(&src);
    Tundra_DynStkint_free(&dst);
}
TEST_END

TEST_BEGIN(move)
{
    Tundra_DynamicStackint src;
    Tundra_DynamicStackint dst;

    constexpr int NUM_INIT_ELEM = 5;
    constexpr int CAP_SIZE = 8;
    constexpr int elems[NUM_INIT_ELEM] = {1, 2, 3, 4, 5};

    Tundra_DynStkint_init_w_elems(&src, elems, NUM_INIT_ELEM, false);

    // dst purposely left uninitialized.
    Tundra_DynStkint_move(&src, &dst);

    assert(dst.cap == CAP_SIZE);
    assert(dst.num_elem == NUM_INIT_ELEM);
    assert(src.data == nullptr);

    for(int i = 0; i < NUM_INIT_ELEM; ++i)
    {
        assert(dst.data[i] == elems[i]);
    }

    Tundra_DynStkint_free(&dst);
}
TEST_END

TEST_BEGIN(clear)
{
    Tundra_DynamicStackint stk;

    constexpr int NUM_INIT_ELEM = 5;
    constexpr int elems[NUM_INIT_ELEM] = {1, 2, 3, 4, 5};

    Tundra_DynStkint_init_w_elems(&stk, elems, NUM_INIT_ELEM, false);

    Tundra_DynStkint_clear(&stk);

    assert(stk.num_elem == 0);
    assert(stk.cap == 8);

    Tundra_DynStkint_free(&stk);
}
TEST_END

TEST_BEGIN(push_pop)
{
    Tundra_DynamicStackint stk;
    Tundra_DynStkint_init(&stk);

    Tundra_DynStkint_push(&stk, 1);
    Tundra_DynStkint_push(&stk, 2);

    assert(stk.num_elem == 2);
    assert(stk.data[0] == 1);
    assert(stk.data[1] == 2);

    Tundra_DynStkint_pop(&stk);

    assert(stk.num_elem == 1);
    assert(stk.data[0] == 1);

    Tundra_DynStkint_free(&stk);
}
TEST_END

TEST_BEGIN(reserve)
{
    Tundra_DynamicStackint stk;
    Tundra_DynStkint_init(&stk);

    assert(stk.cap == TUNDRA_DYNSTK_DEF_CAP);

    Tundra_DynStkint_reserve(&stk, 20);

    assert(stk.cap == 32);

    Tundra_DynStkint_free(&stk);
}
TEST_END

TEST_BEGIN(shrink)
{
    constexpr int NUM_INIT_ELEM = 5;
    constexpr int elems[NUM_INIT_ELEM] = {1, 2, 3, 4, 5};

    Tundra_DynamicStackint stk;
    Tundra_DynStkint_init_w_elems(&stk, elems, NUM_INIT_ELEM, false);

    assert(stk.num_elem == NUM_INIT_ELEM);
    assert(stk.cap == 8);

    Tundra_DynStkint_shrink_to_fit(&stk);

    assert(stk.num_elem == NUM_INIT_ELEM);
    assert(stk.cap == NUM_INIT_ELEM);

    Tundra_DynStkint_shrink_to_new_cap(&stk, 2);

    assert(stk.num_elem == 2);
    assert(stk.cap == 2);

    Tundra_DynStkint_free(&stk);
}
TEST_END

TEST_BEGIN(empty_size_cap)
{
    Tundra_DynamicStackint stk;
    Tundra_DynStkint_init(&stk);

    assert(stk.num_elem == 0);
    assert(Tundra_DynStkint_is_empty(&stk));

    Tundra_DynStkint_push(&stk, 3);

    assert(stk.num_elem == 1);
    assert(Tundra_DynStkint_size(&stk) == 1);

    assert(stk.cap == TUNDRA_DYNSTK_DEF_CAP);
    assert(Tundra_DynStkint_capacity(&stk) == TUNDRA_DYNSTK_DEF_CAP);

    Tundra_DynStkint_free(&stk);
}
TEST_END
#include <iostream>
TEST_BEGIN(top)
{
    constexpr int NUM_INIT_ELEM = 5;
    constexpr int elems[NUM_INIT_ELEM] = {1, 2, 3, 4, 5};

    Tundra_DynamicStackint stk;
    Tundra_DynStkint_init_w_elems(&stk, elems, NUM_INIT_ELEM, false);

    assert(stk.data[4] == 5);
    assert(*Tundra_DynStkint_front(&stk) == 5);

    // Const Test

    assert(*Tundra_DynStkint_front_cst(&stk) == 5);

    Tundra_DynStkint_free(&stk);
}
TEST_END

int main()
{   
    std::cout << "DynStkTest: \n";

    assert(Tundra_init() == 0);

    run_all_tests();

    std::cout << "\n";
    return 0;
}