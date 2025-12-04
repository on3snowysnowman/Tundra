/**
 * @file LnkLstTest.cpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Test file for testing the LinkedList container.
 * @date 2025-11-23
 * 
 * @copyright Copyright (c) 2025
*/

#include <cassert>

#include "tundra/utils/containers/LinkedList.hpp"
#include "../TestingHelper.hpp"
#include "tundra/Tundra.hpp"

TEST_BEGIN(init) // NOLINT
{
    // Default init method
    Tundra::LnkLst::LinkedList<int> lst_one;
    Tundra::LnkLst::init(lst_one);

    assert(lst_one.cap == Tundra::LnkLst::Internal::DEF_CAP);
    assert(lst_one.num_node == 0);
    assert(lst_one.nodes);
    assert(lst_one.head_idx == Tundra::LnkLst::Internal::SENTINEL_IDX);
    assert(lst_one.tail_idx == Tundra::LnkLst::Internal::SENTINEL_IDX);

    // Specified capacity init method
    Tundra::LnkLst::LinkedList<int> lst_two;
    Tundra::LnkLst::init(lst_two, 10);

    assert(lst_two.cap == 10);
    assert(lst_two.num_node == 0);
    assert(lst_two.nodes);
    assert(lst_two.head_idx == Tundra::LnkLst::Internal::SENTINEL_IDX);
    assert(lst_two.tail_idx == Tundra::LnkLst::Internal::SENTINEL_IDX);

    // Initial elements init method
    constexpr int NUM_INIT_ELEM = 5;
    constexpr int elems[NUM_INIT_ELEM] = {1, 2, 3, 4, 5};

    Tundra::LnkLst::LinkedList<int> lst_thr;

    // Test without strict alloc
    Tundra::LnkLst::init(lst_thr, elems, NUM_INIT_ELEM);

    assert(lst_thr.cap == 10);
    assert(lst_thr.num_node == NUM_INIT_ELEM);
    assert(lst_thr.nodes);
    assert(lst_thr.head_idx == 1);
    assert(lst_thr.tail_idx == NUM_INIT_ELEM);

    // Check first Node
    assert(lst_thr.nodes[1].datum == elems[0]);
    assert(lst_thr.nodes[1].next == 2);
    assert(lst_thr.nodes[1].prev == Tundra::LnkLst::Internal::SENTINEL_IDX);

    // Check second Node to one from last node
    for(int i = 1; i < NUM_INIT_ELEM - 1; ++i)
    {
        assert(lst_thr.nodes[i + 1].datum == elems[i]);
        assert(lst_thr.nodes[i + 1].next == i + 2);
        assert(lst_thr.nodes[i + 1].prev == i);
    }

    // Check last Node
    assert(lst_thr.nodes[NUM_INIT_ELEM].datum == elems[NUM_INIT_ELEM - 1]);
    assert(lst_thr.nodes[NUM_INIT_ELEM].next == 
        Tundra::LnkLst::Internal::SENTINEL_IDX);
    assert(lst_thr.nodes[NUM_INIT_ELEM].prev == 
        NUM_INIT_ELEM - 1);

    Tundra::LnkLst::free(lst_thr);

    // Test with strict alloc
    Tundra::LnkLst::init(lst_thr, elems, NUM_INIT_ELEM, true);

    // Add one here to account for the sentinel
    assert(lst_thr.cap == NUM_INIT_ELEM + 1);
    assert(lst_thr.num_node == NUM_INIT_ELEM);
    assert(lst_thr.nodes);
    assert(lst_thr.head_idx == 1);
    assert(lst_thr.tail_idx == NUM_INIT_ELEM);

    // Check first Node
    assert(lst_thr.nodes[1].datum == elems[0]);
    assert(lst_thr.nodes[1].next == 2);
    assert(lst_thr.nodes[1].prev == Tundra::LnkLst::Internal::SENTINEL_IDX);

    // Check second Node to one from last node
    for(int i = 1; i < NUM_INIT_ELEM - 1; ++i)
    {
        assert(lst_thr.nodes[i + 1].datum == elems[i]);
        assert(lst_thr.nodes[i + 1].next == i + 2);
        assert(lst_thr.nodes[i + 1].prev == i);
    }

    // Check last Node
    assert(lst_thr.nodes[NUM_INIT_ELEM].datum == elems[NUM_INIT_ELEM - 1]);
    assert(lst_thr.nodes[NUM_INIT_ELEM].next == 
        Tundra::LnkLst::Internal::SENTINEL_IDX);
    assert(lst_thr.nodes[NUM_INIT_ELEM].prev == 
        NUM_INIT_ELEM - 1);

    Tundra::LnkLst::free(lst_one);
    Tundra::LnkLst::free(lst_two);
    Tundra::LnkLst::free(lst_thr);
}
TEST_END

TEST_BEGIN(free)
{
    Tundra::LnkLst::LinkedList<int> lst;
    Tundra::LnkLst::init(lst);
    Tundra::LnkLst::free(lst);

    assert(lst.nodes == nullptr);
}
TEST_END

TEST_BEGIN(copy)
{
    Tundra::LnkLst::LinkedList<int> src;
    Tundra::LnkLst::LinkedList<int> dst;

    constexpr int NUM_INIT_ELEM = 5;
    constexpr int elems[NUM_INIT_ELEM] = {1, 2, 3, 4, 5};

    Tundra::LnkLst::init(src, elems, NUM_INIT_ELEM);

    // dst purposely left uninitialized.
    Tundra::LnkLst::copy(src, dst);

    assert(dst.cap == src.cap);
    assert(dst.num_node == src.num_node);
    assert(dst.head_idx == src.head_idx);
    assert(dst.tail_idx == src.tail_idx);
    assert(dst.nodes != src.nodes);

    Tundra::uint64 idx = dst.head_idx;

    while(idx != Tundra::LnkLst::Internal::SENTINEL_IDX)
    {
        assert(dst.nodes[idx].datum == src.nodes[idx].datum);
        assert(dst.nodes[idx].next == src.nodes[idx].next);
        assert(dst.nodes[idx].prev == src.nodes[idx].prev);
        idx = dst.nodes[idx].next;
    }

    Tundra::LnkLst::free(src);
    Tundra::LnkLst::free(dst);
}
TEST_END


int main()
{
    assert(Tundra::init_tundra() == 0);
    run_all_tests();
    return 0;
}