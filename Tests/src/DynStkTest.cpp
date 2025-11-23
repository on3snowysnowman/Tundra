/**
 * @file DynStkTest.cpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Test file for testing the DynamicStack container.
 * @date 2025-11-23
 * 
 * @copyright Copyright (c) 2025
*/

#include <cassert>

#include "tundra/utils/containers/DynamicStack.hpp"
#include "../TestingHelper.hpp"
#include "tundra/Tundra.hpp"


TEST_BEGIN(init)
{
    Tundra::DynStk::DynamicStack<int> stk_one;

    // Default init method
    Tundra::DynStk::init(stk_one);

    assert(stk_one.cap == Tundra::DynStk::Internal::DEF_CAP);
    assert(stk_one.num_elem == 0);
    assert(stk_one.data);

    // Specified capacity init method
    Tundra::DynStk::DynamicStack<int> stk_two;
    Tundra::DynStk::init(stk_two, 10);

    assert(stk_two.cap == 10);
    assert(stk_two.num_elem == 0);
    assert(stk_two.data);

    // Initial elements init method.
    constexpr int NUM_INIT_ELEM = 5;
    constexpr int elems[NUM_INIT_ELEM] = {1, 2, 3, 4, 5};

    Tundra::DynStk::DynamicStack<int> stk_thr;

    // Test without strict alloc
    Tundra::DynStk::init(stk_thr, elems, NUM_INIT_ELEM);
    
    assert(stk_thr.cap == 8);
    assert(stk_thr.num_elem == NUM_INIT_ELEM);
    assert(stk_thr.data);

    for(int i = 0; i < NUM_INIT_ELEM; ++i)
    {
        assert(stk_thr.data[i] == elems[i]);
    }

    Tundra::DynStk::free(stk_thr);

    // Test with strict alloc
    Tundra::DynStk::init(stk_thr, elems, NUM_INIT_ELEM, true);

    assert(stk_thr.cap == NUM_INIT_ELEM);
    assert(stk_thr.num_elem == NUM_INIT_ELEM);
    assert(stk_thr.data);

    for(int i = 0; i < NUM_INIT_ELEM; ++i)
    {
        assert(stk_thr.data[i] == elems[i]);
    }

    Tundra::DynStk::free(stk_one);
    Tundra::DynStk::free(stk_two);
    Tundra::DynStk::free(stk_thr);
}
TEST_END

TEST_BEGIN(free)
{
    Tundra::DynStk::DynamicStack<int> stk;
    Tundra::DynStk::init(stk);
    Tundra::DynStk::free(stk);

    assert(stk.data == nullptr);
}
TEST_END

TEST_BEGIN(copy)
{
    Tundra::DynStk::DynamicStack<int> src;
    Tundra::DynStk::DynamicStack<int> dst;

    constexpr int NUM_INIT_ELEM = 5;
    constexpr int elems[NUM_INIT_ELEM] = {1, 2, 3, 4, 5};

    Tundra::DynStk::init(src, elems, NUM_INIT_ELEM);

    // dst purposely left uninitialized.
    Tundra::DynStk::copy(src, dst);

    assert(dst.cap == src.cap);
    assert(dst.num_elem == src.num_elem);
    assert(dst.data != src.data);

    for(int i = 0; i < NUM_INIT_ELEM; ++i)
    {
        assert(dst.data[i] == elems[i]);
    }

    Tundra::DynStk::free(src);
    Tundra::DynStk::free(dst);
}
TEST_END

TEST_BEGIN(move)
{
    Tundra::DynStk::DynamicStack<int> src;
    Tundra::DynStk::DynamicStack<int> dst;

    constexpr int NUM_INIT_ELEM = 5;
    constexpr int CAP_SIZE = 8;
    constexpr int elems[NUM_INIT_ELEM] = {1, 2, 3, 4, 5};

    Tundra::DynStk::init(src, elems, NUM_INIT_ELEM);

    // dst purposely left uninitialized.
    Tundra::DynStk::move(src, dst);

    assert(dst.cap == CAP_SIZE);
    assert(dst.num_elem == NUM_INIT_ELEM);
    assert(src.data == nullptr);

    for(int i = 0; i < NUM_INIT_ELEM; ++i)
    {
        assert(dst.data[i] == elems[i]);
    }

    Tundra::DynStk::free(dst);
}
TEST_END

TEST_BEGIN(clear)
{
    Tundra::DynStk::DynamicStack<int> stk;

    constexpr int NUM_INIT_ELEM = 5;
    constexpr int elems[NUM_INIT_ELEM] = {1, 2, 3, 4, 5};

    Tundra::DynStk::init(stk, elems, NUM_INIT_ELEM);

    Tundra::DynStk::clear(stk);

    assert(stk.num_elem == 0);
    assert(stk.cap == 8);

    Tundra::DynStk::free(stk);
}
TEST_END

TEST_BEGIN(push_pop)
{
    Tundra::DynStk::DynamicStack<int> stk;
    Tundra::DynStk::init(stk);

    Tundra::DynStk::push(stk, 1);
    Tundra::DynStk::push(stk, 2);

    assert(stk.num_elem == 2);
    assert(stk.data[0] == 1);
    assert(stk.data[1] == 2);

    Tundra::DynStk::pop(stk);

    assert(stk.num_elem == 1);
    assert(stk.data[0] == 1);

    Tundra::DynStk::free(stk);
}
TEST_END

TEST_BEGIN(resize)
{   
    Tundra::DynStk::DynamicStack<int> stk;
    Tundra::DynStk::init(stk);

    Tundra::DynStk::push(stk, 1);
    Tundra::DynStk::push(stk, 2);
    Tundra::DynStk::push(stk, 3);

    assert(stk.num_elem == 3);
    assert(stk.cap == 4);

    Tundra::DynStk::resize(stk, 9);

    assert(stk.num_elem == 9);
    assert(stk.cap == 16);

    Tundra::DynStk::free(stk);
}
TEST_END

TEST_BEGIN(shrink)
{
    constexpr int NUM_INIT_ELEM = 5;
    constexpr int elems[NUM_INIT_ELEM] = {1, 2, 3, 4, 5};

    Tundra::DynStk::DynamicStack<int> stk;
    Tundra::DynStk::init(stk, elems, NUM_INIT_ELEM);

    assert(stk.num_elem == NUM_INIT_ELEM);
    assert(stk.cap == 8);

    Tundra::DynStk::shrink_to_fit(stk);

    assert(stk.num_elem == NUM_INIT_ELEM);
    assert(stk.cap == NUM_INIT_ELEM);

    Tundra::DynStk::shrink_to_new_cap(stk, 2);

    assert(stk.num_elem == 2);
    assert(stk.cap == 2);

    Tundra::DynStk::free(stk);
}
TEST_END

TEST_BEGIN(empty_size_cap)
{
    Tundra::DynStk::DynamicStack<int> stk;
    Tundra::DynStk::init(stk);

    assert(stk.num_elem == 0);
    assert(Tundra::DynStk::is_empty(stk));

    Tundra::DynStk::push(stk, 3);

    assert(stk.num_elem == 1);
    assert(Tundra::DynStk::size(stk) == 1);

    assert(stk.cap == Tundra::DynStk::Internal::DEF_CAP);
    assert(Tundra::DynStk::capacity(stk) == Tundra::DynStk::Internal::DEF_CAP);

    Tundra::DynStk::free(stk);
}
TEST_END

TEST_BEGIN(top)
{
    constexpr int NUM_INIT_ELEM = 5;
    constexpr int elems[NUM_INIT_ELEM] = {1, 2, 3, 4, 5};

    Tundra::DynStk::DynamicStack<int> stk;
    Tundra::DynStk::init(stk, elems, NUM_INIT_ELEM);

    assert(stk.data[4] == 5);
    assert(Tundra::DynStk::top(stk) == 5);

    // Const Test
    const Tundra::DynStk::DynamicStack<int> &stk_ref = stk;

    assert(Tundra::DynStk::top(stk) == 5);

    Tundra::DynStk::free(stk);
}
TEST_END

int main()
{   
    assert(Tundra::init_tundra() == 0);
    run_all_tests();
    return 0;
}