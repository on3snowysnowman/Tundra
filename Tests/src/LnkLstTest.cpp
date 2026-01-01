/**
 * @file LnkLstTest.cpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Test file for testing the LinkedList container.
 * @version 0.1
 * @date 2025-12-31
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include <iostream>
#include <cassert>
#include <random>

#include "../TestingHelper.hpp"
#include "tundra/utils/Math.h"
#include "tundra/containers/LinkedListint.h"

static constexpr uint64 LIST_NODE_SIZE = sizeof(InTundra_LinkedListNodeint);

#define CALC_CAP_BYTES(num_elem) \
    Tundra_ceil_pow2((num_elem) * LIST_NODE_SIZE)

static constexpr int TEST_ITERATIONS = 50;

/**
 * @brief Returns a random integer between `min` and `max`.
 * 
 * @param min 
 * @param max 
 * @return int 
 */
int get_rand_int(int min, int max)
{
    static std::mt19937 rng{ std::random_device{}() };
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}


TEST_BEGIN(basic_init)
{
    Tundra_LinkedListint list;
    Tundra_LnkLstint_init(&list);

    assert(list.num_node == 0);
    assert(list.head_idx == TUNDRA_LNKLST_SENTINEL_IDX);
    assert(list.tail_idx == TUNDRA_LNKLST_SENTINEL_IDX);
    assert(list.cap_bytes == CALC_CAP_BYTES(TUNDRA_LNKLST_DEF_CAP));
    assert(list.cap == list.cap_bytes / LIST_NODE_SIZE);
    assert(list.nodes);

    Tundra_LnkLstint_free(&list);
}
TEST_END

TEST_BEGIN(cap_init)
{
    for(int i = 0; i < TEST_ITERATIONS; ++i)
    {   
        int capacity = get_rand_int(1, 33);

        Tundra_LinkedListint list;
        Tundra_LnkLstint_init_w_cap(&list, capacity);

        int expected_cap_bytes = CALC_CAP_BYTES(capacity);

        assert(list.num_node == 0);
        assert(list.head_idx == TUNDRA_LNKLST_SENTINEL_IDX);
        assert(list.tail_idx == TUNDRA_LNKLST_SENTINEL_IDX);
        assert(list.cap_bytes == expected_cap_bytes);
        assert(list.cap == list.cap_bytes / LIST_NODE_SIZE);
        assert(list.nodes);

        Tundra_LnkLstint_free(&list);
    }
}
TEST_END

TEST_BEGIN(elem_init)
{
    for(int i = 0; i < TEST_ITERATIONS; ++i)
    {
        // Init initial elements
        int num_elem = get_rand_int(1, 15);
        int *init_elems = new int[num_elem];

        for(int j = 0; j < num_elem; ++j)
        {
            init_elems[j] = get_rand_int(-100, 100);
        }

        // Init List
        Tundra_LinkedListint list;
        Tundra_LnkLstint_init_w_elems(&list, init_elems, num_elem);

        // Test correctness
        int expected_cap_bytes = CALC_CAP_BYTES(num_elem + 1);

        assert(list.num_node == num_elem);
        assert(list.head_idx == 1);
        assert(list.tail_idx == num_elem);
        assert(list.cap_bytes == expected_cap_bytes);
        assert(list.cap == list.cap_bytes / LIST_NODE_SIZE);
        assert(list.nodes);

        // Check each element.
        const InTundra_LinkedListNodeint *parsed_node = 
            list.nodes + list.head_idx;

        int j = 0;
        while(parsed_node != list.nodes + TUNDRA_LNKLST_SENTINEL_IDX)
        {
            assert(parsed_node->datum == init_elems[j]);

            parsed_node = list.nodes + parsed_node->next;
            ++j;
        }

        // Ensure we made it through each element.
        assert(j == num_elem);

        Tundra_LnkLstint_free(&list);

        delete[] init_elems;
    }
}
TEST_END

TEST_BEGIN(copy_init)
{
    for(int i = 0; i < TEST_ITERATIONS; ++i)
    {
        // Init initial elements
        int num_elem = get_rand_int(1, 15);
        int *init_elems = new int[num_elem];

        for(int j = 0; j < num_elem; ++j)
        {
            init_elems[j] = get_rand_int(-100, 100);
        }

        // Init source List.
        Tundra_LinkedListint src;
        Tundra_LnkLstint_init_w_elems(&src, init_elems, num_elem);

        // Init dest List
        Tundra_LinkedListint dst;
        Tundra_LnkLstint_init_w_copy(&src, &dst);

        // Test correctness
        assert(dst.num_node == src.num_node);
        assert(dst.head_idx == src.head_idx);
        assert(dst.tail_idx == src.tail_idx);
        assert(dst.cap_bytes == src.cap_bytes);
        assert(dst.cap == src.cap);
        assert(dst.nodes && dst.nodes != src.nodes);

        // Check each Node

        const InTundra_LinkedListNodeint *src_parsed = src.nodes + src.head_idx;
        const InTundra_LinkedListNodeint *dst_parsed = dst.nodes + dst.head_idx;

        while(dst_parsed != dst.nodes + TUNDRA_LNKLST_SENTINEL_IDX)
        {
            // Ensure that source parsed Node has not reached the Sentinel
            assert(src_parsed != src.nodes + TUNDRA_LNKLST_SENTINEL_IDX);

            assert(src_parsed->datum == dst_parsed->datum);

            src_parsed = src.nodes + src_parsed->next;
            dst_parsed = dst.nodes + dst_parsed->next;
        }

        Tundra_LnkLstint_free(&src);    
        Tundra_LnkLstint_free(&dst);
        
        delete[] init_elems;
    }
}
TEST_END

TEST_BEGIN(move_init)
{
    for(int i = 0; i < TEST_ITERATIONS; ++i)
    {
        // Init initial elements
        int num_elem = get_rand_int(1, 15);
        int *init_elems = new int[num_elem];

        for(int j = 0; j < num_elem; ++j)
        {
            init_elems[j] = get_rand_int(-100, 100);
        }

        // Init source List 
        Tundra_LinkedListint src;
        Tundra_LnkLstint_init_w_elems(&src, init_elems, num_elem);

        uint64 saved_num_node = src.num_node;
        uint64 saved_head_idx = src.head_idx;
        uint64 saved_tail_idx = src.tail_idx;
        uint64 saved_cap_bytes = src.cap_bytes;
        uint64 saved_cap = src.cap;
        InTundra_LinkedListNodeint *saved_nodes = src.nodes;

        // Init dest List
        Tundra_LinkedListint dst;
        Tundra_LnkLstint_init_w_move(&src, &dst);

        // Test correctness
        assert(dst.num_node == saved_num_node);
        assert(dst.head_idx == saved_head_idx);
        assert(dst.tail_idx == saved_tail_idx);
        assert(dst.cap_bytes == saved_cap_bytes);
        assert(dst.cap == saved_cap);
        assert(dst.nodes == saved_nodes);

        assert(src.num_node == 0);
        assert(src.head_idx == TUNDRA_LNKLST_SENTINEL_IDX);
        assert(src.tail_idx == TUNDRA_LNKLST_SENTINEL_IDX);
        assert(src.cap_bytes == 0);
        assert(src.cap == 0);
        assert(src.nodes == NULL);
    
        // Check each element.
        int j = 0;
        InTundra_LinkedListNodeint *parsed_node = dst.nodes + dst.head_idx;

        while(parsed_node != dst.nodes + TUNDRA_LNKLST_SENTINEL_IDX)
        {
            assert(parsed_node->datum == init_elems[j]);

            parsed_node = dst.nodes + parsed_node->next;
            ++j;
        }

        Tundra_LnkLstint_free(&dst);

        delete[] init_elems;
    }
}
TEST_END

TEST_BEGIN(free)
{
    Tundra_LinkedListint list;
    Tundra_LnkLstint_init(&list);

    Tundra_LnkLstint_free(&list);
    assert(list.num_node == 0);
    assert(list.head_idx == TUNDRA_LNKLST_SENTINEL_IDX);
    assert(list.tail_idx == TUNDRA_LNKLST_SENTINEL_IDX);
    assert(list.cap_bytes == 0);
    assert(list.cap == 0);
    assert(list.nodes == NULL);
}
TEST_END

TEST_BEGIN(copy)
{
    for(int i = 0; i < TEST_ITERATIONS; ++i)
    {
        // Init initial elements
        int num_elem = get_rand_int(1, 15);
        int *init_elems = new int[num_elem];

        for(int j = 0; j < num_elem; ++j)
        {
            init_elems[j] = get_rand_int(-100, 100);
        }

        // Init source List.
        Tundra_LinkedListint src;
        Tundra_LnkLstint_init_w_elems(&src, init_elems, num_elem);

        // Init dest List
        Tundra_LinkedListint dst;
        Tundra_LnkLstint_init(&dst);

        Tundra_LnkLstint_copy(&src, &dst);

        // Test correctness
        assert(dst.num_node == src.num_node);
        assert(dst.head_idx == src.head_idx);
        assert(dst.tail_idx == src.tail_idx);
        assert(dst.cap_bytes == src.cap_bytes);
        assert(dst.cap == src.cap);
        assert(dst.nodes && dst.nodes != src.nodes);

        // Check each Node

        const InTundra_LinkedListNodeint *src_parsed = src.nodes + src.head_idx;
        const InTundra_LinkedListNodeint *dst_parsed = dst.nodes + dst.head_idx;

        while(dst_parsed != dst.nodes + TUNDRA_LNKLST_SENTINEL_IDX)
        {
            // Ensure that source parsed Node has not reached the Sentinel
            assert(src_parsed != src.nodes + TUNDRA_LNKLST_SENTINEL_IDX);

            assert(src_parsed->datum == dst_parsed->datum);

            src_parsed = src.nodes + src_parsed->next;
            dst_parsed = dst.nodes + dst_parsed->next;
        }

        Tundra_LnkLstint_free(&src);    
        Tundra_LnkLstint_free(&dst);
        
        delete[] init_elems;
    }
}
TEST_END

TEST_BEGIN(move)
{
    for(int i = 0; i < TEST_ITERATIONS; ++i)
    {
        // Init initial elements
        int num_elem = get_rand_int(1, 15);
        int *init_elems = new int[num_elem];

        for(int j = 0; j < num_elem; ++j)
        {
            init_elems[j] = get_rand_int(-100, 100);
        }

        // Init source List 
        Tundra_LinkedListint src;
        Tundra_LnkLstint_init_w_elems(&src, init_elems, num_elem);

        uint64 saved_num_node = src.num_node;
        uint64 saved_head_idx = src.head_idx;
        uint64 saved_tail_idx = src.tail_idx;
        uint64 saved_cap_bytes = src.cap_bytes;
        uint64 saved_cap = src.cap;
        InTundra_LinkedListNodeint *saved_nodes = src.nodes;

        // Init dest List
        Tundra_LinkedListint dst;
        Tundra_LnkLstint_init(&dst);

        Tundra_LnkLstint_move(&src, &dst);

        // Test correctness
        assert(dst.num_node == saved_num_node);
        assert(dst.head_idx == saved_head_idx);
        assert(dst.tail_idx == saved_tail_idx);
        assert(dst.cap_bytes == saved_cap_bytes);
        assert(dst.cap == saved_cap);
        assert(dst.nodes == saved_nodes);

        assert(src.num_node == 0);
        assert(src.head_idx == TUNDRA_LNKLST_SENTINEL_IDX);
        assert(src.tail_idx == TUNDRA_LNKLST_SENTINEL_IDX);
        assert(src.cap_bytes == 0);
        assert(src.cap == 0);
        assert(src.nodes == NULL);
    
        // Check each element.
        int j = 0;
        InTundra_LinkedListNodeint *parsed_node = dst.nodes + dst.head_idx;

        while(parsed_node != dst.nodes + TUNDRA_LNKLST_SENTINEL_IDX)
        {
            assert(parsed_node->datum == init_elems[j]);

            parsed_node = dst.nodes + parsed_node->next;
            ++j;
        }

        Tundra_LnkLstint_free(&dst);

        delete[] init_elems;
    }
}
TEST_END

TEST_MAIN(LnkLstTest)
