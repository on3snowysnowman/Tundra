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

#define LIST_SENTINEL_IDX 0

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
    assert(list.nodes[LIST_SENTINEL_IDX].next == 
        LIST_SENTINEL_IDX);
    assert(list.nodes[LIST_SENTINEL_IDX].prev == 
        LIST_SENTINEL_IDX);
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

        uint64 expected_cap_bytes = CALC_CAP_BYTES(capacity);

        assert(list.num_node == 0);
        assert(list.nodes[LIST_SENTINEL_IDX].next == 
            LIST_SENTINEL_IDX);
        assert(list.nodes[LIST_SENTINEL_IDX].prev == 
            LIST_SENTINEL_IDX);
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
        uint64 expected_cap_bytes = CALC_CAP_BYTES(num_elem + 1);

        assert(list.num_node == (uint64)num_elem);
        assert(list.nodes[LIST_SENTINEL_IDX].next == 1);
        assert(list.nodes[LIST_SENTINEL_IDX].prev == (uint64)num_elem);
        assert(list.cap_bytes == expected_cap_bytes);
        assert(list.cap == list.cap_bytes / LIST_NODE_SIZE);
        assert(list.nodes);

        // Check each element. Start at the head Node which is the next value of
        // the Sentinel.
        const InTundra_LinkedListNodeint *parsed_node = 
            &list.nodes[list.nodes[LIST_SENTINEL_IDX].next];

        int j = 0;
        while(parsed_node != list.nodes + LIST_SENTINEL_IDX)
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
        // assert(dst.head_idx == src.head_idx);
        // assert(dst.tail_idx == src.tail_idx);
        assert(dst.cap_bytes == src.cap_bytes);
        assert(dst.cap == src.cap);
        assert(dst.nodes && dst.nodes != src.nodes);

        // Check each Node

        const InTundra_LinkedListNodeint *src_parsed = 
            &src.nodes[src.nodes[LIST_SENTINEL_IDX].next];
        const InTundra_LinkedListNodeint *dst_parsed = 
            &dst.nodes[dst.nodes[LIST_SENTINEL_IDX].next];

        while(dst_parsed != dst.nodes + LIST_SENTINEL_IDX)
        {
            // Ensure that source parsed Node has not reached the Sentinel
            assert(src_parsed != src.nodes + LIST_SENTINEL_IDX);

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
        uint64 saved_head_idx = src.nodes[LIST_SENTINEL_IDX].next;
        uint64 saved_tail_idx = src.nodes[LIST_SENTINEL_IDX].prev;
        uint64 saved_cap_bytes = src.cap_bytes;
        uint64 saved_cap = src.cap;
        InTundra_LinkedListNodeint *saved_nodes = src.nodes;

        // Init dest List
        Tundra_LinkedListint dst;
        Tundra_LnkLstint_init_w_move(&src, &dst);

        // Test correctness
        assert(dst.num_node == saved_num_node);
        assert(dst.nodes[LIST_SENTINEL_IDX].next == saved_head_idx);
        assert(dst.nodes[LIST_SENTINEL_IDX].prev == saved_tail_idx);
        assert(dst.cap_bytes == saved_cap_bytes);
        assert(dst.cap == saved_cap);
        assert(dst.nodes == saved_nodes);

        assert(src.num_node == 0);
        assert(src.cap_bytes == 0);
        assert(src.cap == 0);
        assert(src.nodes == NULL);
    
        // Check each element.
        int j = 0;
        InTundra_LinkedListNodeint *parsed_node = dst.nodes;

        while(parsed_node != dst.nodes + LIST_SENTINEL_IDX)
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
        assert(dst.nodes[LIST_SENTINEL_IDX].next == 
            src.nodes[LIST_SENTINEL_IDX].next);
        assert(dst.nodes[LIST_SENTINEL_IDX].prev == 
            src.nodes[LIST_SENTINEL_IDX].prev);
        assert(dst.cap_bytes == src.cap_bytes);
        assert(dst.cap == src.cap);
        assert(dst.nodes && dst.nodes != src.nodes);

        // Check each Node
        const InTundra_LinkedListNodeint *src_parsed = 
            &src.nodes[src.nodes[LIST_SENTINEL_IDX].next];
        const InTundra_LinkedListNodeint *dst_parsed = 
            &dst.nodes[dst.nodes[LIST_SENTINEL_IDX].next];

        while(dst_parsed != dst.nodes + LIST_SENTINEL_IDX)
        {
            // Ensure that source parsed Node has not reached the Sentinel
            assert(src_parsed != src.nodes + LIST_SENTINEL_IDX);

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
        uint64 saved_head_idx = src.nodes[LIST_SENTINEL_IDX].next;
        uint64 saved_tail_idx = src.nodes[LIST_SENTINEL_IDX].prev;
        uint64 saved_cap_bytes = src.cap_bytes;
        uint64 saved_cap = src.cap;
        InTundra_LinkedListNodeint *saved_nodes = src.nodes;

        // Init dest List
        Tundra_LinkedListint dst;
        Tundra_LnkLstint_init(&dst);

        Tundra_LnkLstint_move(&src, &dst);

        // Test correctness
        assert(dst.num_node == saved_num_node);
        assert(dst.nodes[LIST_SENTINEL_IDX].next == saved_head_idx);
        assert(dst.nodes[LIST_SENTINEL_IDX].prev == saved_tail_idx);
        assert(dst.cap_bytes == saved_cap_bytes);
        assert(dst.cap == saved_cap);
        assert(dst.nodes == saved_nodes);

        assert(src.num_node == 0);
        assert(src.cap_bytes == 0);
        assert(src.cap == 0);
        assert(src.nodes == NULL);
    
        // Check each element.
        int j = 0;
        InTundra_LinkedListNodeint *parsed_node = dst.nodes;

        while(parsed_node != dst.nodes + LIST_SENTINEL_IDX)
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

TEST_BEGIN(add_front)
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

        int elem_to_add = get_rand_int(-100, 100);

        Tundra_LnkLstint_add_front(&list, &elem_to_add);

        // Test correctness

        // +2 for the Sentinel and the added element.
        uint64 expected_cap_bytes = CALC_CAP_BYTES(num_elem + 2);

        // The index of the newly added node should be 1 more than the initial 
        // number of elements we added in.
        const uint64 ADDED_NODE_IDX = num_elem + 1;

        assert(list.num_node == (uint64)num_elem + 1);
        assert(list.nodes[LIST_SENTINEL_IDX].next == ADDED_NODE_IDX);
        assert(list.nodes[LIST_SENTINEL_IDX].prev == (uint64)num_elem);
        assert(list.cap_bytes == expected_cap_bytes);
        assert(list.cap == list.cap_bytes / LIST_NODE_SIZE);
        assert(list.nodes);

        // -- Check each Node in the List -- 

        const InTundra_LinkedListNodeint *node =     
            &list.nodes[list.nodes[LIST_SENTINEL_IDX].next];

        // First check the first Node, which we've added.
        assert(node->datum == elem_to_add);

        // -- Iterate through the rest of the Nodes, comparing them to the order 
        // of the initial elems. --
        
        // Iterate to the Node after the head Node.
        node = &list.nodes[node->next];

        for(uint64 i = 0; i < (uint64)num_elem; ++i)
        {
            // Ensure the parsed Node does not equal the Sentinel.
            assert(node != list.nodes);

            assert(node->datum == init_elems[i]);

            node = &list.nodes[node->next];
        }

        Tundra_LnkLstint_free(&list);

        delete[] init_elems;
    }
}
TEST_END

TEST_BEGIN(add_back)
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

        int elem_to_add = get_rand_int(-100, 100);

        Tundra_LnkLstint_add_back(&list, &elem_to_add);

        // Test correctness

        // +2 for the Sentinel and the added element.
        uint64 expected_cap_bytes = CALC_CAP_BYTES(num_elem + 2);

        // The index of the newly added node should be 1 more than the initial 
        // number of elements we added in.
        const uint64 ADDED_NODE_IDX = num_elem + 1;

        assert(list.num_node == (uint64)num_elem + 1);
        assert(list.nodes[LIST_SENTINEL_IDX].next == 1);
        assert(list.nodes[LIST_SENTINEL_IDX].prev == ADDED_NODE_IDX);
        assert(list.cap_bytes == expected_cap_bytes);
        assert(list.cap == list.cap_bytes / LIST_NODE_SIZE);
        assert(list.nodes);

        // -- Check each Node in the List -- 

        const InTundra_LinkedListNodeint *node =     
            &list.nodes[list.nodes[LIST_SENTINEL_IDX].next];

        // -- Iterate through the Nodes before the last Node, comparing them to 
        // the order of the initial elems. --

        for(uint64 i = 0; i < (uint64)num_elem; ++i)
        {
            // Ensure the parsed Node does not equal the Sentinel.
            assert(node != list.nodes);

            assert(node->datum == init_elems[i]);

            node = &list.nodes[node->next];
        }

        // Check the last Node, which should be the element we've added.
        assert(node->next == LIST_SENTINEL_IDX);
        assert(node->datum == elem_to_add); 

        Tundra_LnkLstint_free(&list);

        delete[] init_elems;
    }
}
TEST_END

TEST_BEGIN(insert)
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

        int elem_to_add = get_rand_int(-100, 100);
        uint64 idx_to_insert = get_rand_int(0, num_elem - 1);

        Tundra_LnkLstint_insert(&list, &elem_to_add, idx_to_insert);

        // +2 for the Sentinel and the added element.
        uint64 expected_cap_bytes = CALC_CAP_BYTES(num_elem + 2);

        // Test correctness
        assert(list.num_node == (uint64)num_elem + 1);
        assert(list.cap_bytes == expected_cap_bytes);
        assert(list.cap == list.cap_bytes / LIST_NODE_SIZE);
        assert(list.nodes);

        // -- Check each Node in the List -- 

        const InTundra_LinkedListNodeint *node = 
            &list.nodes[list.nodes[LIST_SENTINEL_IDX].next];

        // Iterate through the Nodes before the insert index
        for(uint64 i = 0; i < idx_to_insert; ++i)
        {
            assert(node->datum == init_elems[i]);
            node = &list.nodes[node->next];
        }

        // Check insert index
        assert(node->datum == elem_to_add);
        node = &list.nodes[node->next];

        // Iterate through Nodes after the insert index
        for(uint64 i = idx_to_insert + 1; i < list.num_node; ++i)
        {
            assert(node->datum == init_elems[i - 1]);
            node = &list.nodes[node->next];
        }

        Tundra_LnkLstint_free(&list);

        delete[] init_elems;
    }   
}
TEST_END

TEST_BEGIN(resize_smaller)
{
    for(int i = 0; i < TEST_ITERATIONS; ++i)
    {
        // Init initial elements
        int num_elem = get_rand_int(2, 40);
        int *init_elems = new int[num_elem];

        for(int j = 0; j < num_elem; ++j)
        {
            init_elems[j] = get_rand_int(-100, 100);
        }


        // Init List
        Tundra_LinkedListint list;
        Tundra_LnkLstint_init_w_elems(&list, init_elems, num_elem);

        const uint64 RESIZE_SIZE = get_rand_int(1, num_elem - 1);

        Tundra_LnkLstint_resize(&list, RESIZE_SIZE);

        // Test correctness. 

        // +1 for Sentinel.
        uint64 expected_cap_bytes = CALC_CAP_BYTES(num_elem + 1);

        assert(list.cap_bytes == expected_cap_bytes);
        assert(list.cap == list.cap_bytes / LIST_NODE_SIZE);
        assert(list.num_node == RESIZE_SIZE);
        assert(list.nodes);
        
        // -- Check elements --

        // Start parsing at the head Node.
        InTundra_LinkedListNodeint *node = 
            &list.nodes[list.nodes[LIST_SENTINEL_IDX].next];

        for(uint64 j = 0; j < RESIZE_SIZE; ++j)
        {
            assert(node->datum == init_elems[j]);

            node = &list.nodes[node->next];
        }

        // Ensure the parsed Node is at the Sentinel, since we should be at the
        // end of the List.
        assert(node == &list.nodes[LIST_SENTINEL_IDX]);

        Tundra_LnkLstint_free(&list);

        delete[] init_elems;
    }
}
TEST_END

TEST_BEGIN(resize_larger)
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

        const uint64 RESIZE_SIZE = list.num_node + get_rand_int(1, 29);

        Tundra_LnkLstint_resize(&list, RESIZE_SIZE);

        // Test correctness

        // +1 for Sentinel.
        uint64 expected_cap_bytes = CALC_CAP_BYTES(RESIZE_SIZE + 1);

        assert(list.cap_bytes == expected_cap_bytes);
        assert(list.cap == list.cap_bytes / LIST_NODE_SIZE);
        assert(list.num_node == RESIZE_SIZE);
        assert(list.nodes);

        // -- Check each element that has a value (not checking the newly added
        // ones due to the resize, since they contain junk as their datum) --
        
        // Start parsing at the head Node.
        InTundra_LinkedListNodeint *node = 
            &list.nodes[list.nodes[LIST_SENTINEL_IDX].next];

        for(uint64 j = 0; j < (uint64)num_elem; ++j)
        {
            assert(node->datum == init_elems[j]);

            node = &list.nodes[node->next];
        }

        // Continue iterating through the List, the position we're at now is
        // the first position of the new Nodes added by the resize call.
        for(uint64 j = num_elem; j < RESIZE_SIZE; ++j)
        {
            // Ensure we haven't reached the Sentinel.
            assert(node != &list.nodes[LIST_SENTINEL_IDX]);

            node = &list.nodes[node->next];
        }

        // Ensure the parsed Node is at the Sentinel, since we should be at the
        // end of the List.
        assert(node == &list.nodes[LIST_SENTINEL_IDX]);

        Tundra_LnkLstint_free(&list);

        delete[] init_elems;
    }
}
TEST_END

TEST_BEGIN(reserve)
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

        const int NUM_RESERVE = get_rand_int(3, 40);

        Tundra_LnkLstint_reserve(&list, NUM_RESERVE);

        // Test correctness

        // +1 for Sentinel
        uint64 expected_cap_bytes = CALC_CAP_BYTES(num_elem + NUM_RESERVE + 1);

        assert(list.cap_bytes == expected_cap_bytes);
        assert(list.cap == expected_cap_bytes / LIST_NODE_SIZE);
        assert(list.num_node == (uint64)num_elem);
        assert(list.nodes);

        Tundra_LnkLstint_free(&list);

        delete[] init_elems;
    }
}
TEST_END

TEST_BEGIN(erase_front)
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

        Tundra_LnkLstint_erase_front(&list);

        // Test correctness 

        // +1 for the Sentinel.
        uint64 expected_cap_bytes = CALC_CAP_BYTES(num_elem + 1);

        assert(list.num_node == (uint64)num_elem - 1);
        assert(list.nodes[LIST_SENTINEL_IDX].next != 1);
        assert(list.cap_bytes == expected_cap_bytes);
        assert(list.cap == list.cap_bytes / LIST_NODE_SIZE);
        assert(list.nodes);

        InTundra_LinkedListNodeint *node = 
            &list.nodes[list.nodes[LIST_SENTINEL_IDX].next];

        // Check each element, starting after the one that was erased.
        for(uint64 i = 0; i < (uint64)num_elem - 1; ++i)
        {
            assert(node->datum == init_elems[i + 1]);
            assert(node != &list.nodes[LIST_SENTINEL_IDX]);
            node = &list.nodes[node->next];
        }

        Tundra_LnkLstint_free(&list);

        delete[] init_elems;
    }
}
TEST_END

TEST_BEGIN(erase_back)
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

        Tundra_LnkLstint_erase_back(&list);

        // Test correctness 

        // +1 for the Sentinel.
        uint64 expected_cap_bytes = CALC_CAP_BYTES(num_elem + 1);

        assert(list.num_node == (uint64)num_elem - 1);
        assert(list.nodes[LIST_SENTINEL_IDX].prev != (uint64)num_elem);
        assert(list.cap_bytes == expected_cap_bytes);
        assert(list.cap == list.cap_bytes / LIST_NODE_SIZE);
        assert(list.nodes);

        InTundra_LinkedListNodeint *node = 
            &list.nodes[list.nodes[LIST_SENTINEL_IDX].next];

        // Check each element up to the last one, which should be erased.
        for(uint64 i = 0; i < (uint64)num_elem - 1; ++i)
        {
            assert(node->datum == init_elems[i]);
            assert(node != &list.nodes[LIST_SENTINEL_IDX]);
            node = &list.nodes[node->next];
        }

        // Test that the last element was erased.
        assert(node == &list.nodes[LIST_SENTINEL_IDX]);

        Tundra_LnkLstint_free(&list);

        delete[] init_elems;
    }
}
TEST_END

TEST_BEGIN(erase_at_index)
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

        int erase_pos = get_rand_int(0, num_elem - 1);

        // Init List
        Tundra_LinkedListint list;
        Tundra_LnkLstint_init_w_elems(&list, init_elems, num_elem);

        Tundra_LnkLstint_erase_at_index(&list, erase_pos);

        // Test correctness

        // +1 for the Sentinel.
        uint64 expected_cap_bytes = CALC_CAP_BYTES(num_elem + 1);

        assert(list.num_node == (uint64)num_elem - 1);
        assert(list.cap_bytes == expected_cap_bytes);
        assert(list.cap == list.cap_bytes / LIST_NODE_SIZE);
        assert(list.nodes);

        InTundra_LinkedListNodeint *node = 
            &list.nodes[list.nodes[LIST_SENTINEL_IDX].next];

        // -- Check elements before the erase position -- 
        for(uint64 i = 0; i < (uint64)erase_pos; ++i)
        {
            assert(node != &list.nodes[LIST_SENTINEL_IDX]);
        
            assert(node->datum == init_elems[i]);

            node = &list.nodes[node->next];
        }

        // -- Check elements after the erase position -- 
        for(uint64 i = erase_pos; i < (uint64)num_elem - 1; ++i)
        {
            assert(node != &list.nodes[LIST_SENTINEL_IDX]);

            assert(node->datum == init_elems[i + 1]);

            node = &list.nodes[node->next];
        }

        Tundra_LnkLstint_free(&list);

        delete[] init_elems;
    }
}
TEST_END

TEST_BEGIN(at)
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

        for(uint64 i = 0; i < (uint64)num_elem; ++i)
        {
            assert(*Tundra_LnkLstint_at(&list, i) == init_elems[i]);
            assert(*Tundra_LnkLstint_at_cst(&list, i) == init_elems[i]);
        }

        Tundra_LnkLstint_free(&list);

        delete[] init_elems;
    }
}
TEST_END

TEST_BEGIN(front_back)
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

        assert(*Tundra_LnkLstint_front(&list) == init_elems[0]);
        assert(*Tundra_LnkLstint_front_cst(&list) == init_elems[0]);
        assert(*Tundra_LnkLstint_back(&list) == init_elems[num_elem - 1]);
        assert(*Tundra_LnkLstint_back_cst(&list) == init_elems[num_elem - 1]);

        Tundra_LnkLstint_free(&list);

        delete[] init_elems;
    }
}
TEST_END

TEST_BEGIN(iter)
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

        Tundra_LinkedListIteratorint begin_it, end_it;

        begin_it = Tundra_LnkLstIterint_begin(&list);

        assert(begin_it.list == &list);
        assert(begin_it.index == 1);

        end_it = Tundra_LnkLstIterint_end(&list);
        assert(end_it.list == &list);
        assert(end_it.index == LIST_SENTINEL_IDX);

        int j = 0;

        while(!Tundra_LnkLstIterint_compare(&begin_it, &end_it))
        {
            assert(*Tundra_LnkLstIterint_deref(&begin_it) == init_elems[j]);
            assert(*Tundra_LnkLstIterint_deref_cst(&begin_it) == init_elems[j]);
            Tundra_LnkLstIterint_next(&begin_it);
            ++j;
        }

        assert(begin_it.index == end_it.index);
        assert(Tundra_LnkLstIterint_compare(&begin_it, &end_it));

        Tundra_LnkLstIterint_prev(&begin_it);

        assert(*Tundra_LnkLstIterint_deref(&begin_it) == 
            init_elems[num_elem - 1]);

        int ran_index = get_rand_int(0, num_elem - 1);

        Tundra_LinkedListIteratorint it_at_idx = 
            Tundra_LnkLstIterint_get_at_index(&list, ran_index);

        assert(*Tundra_LnkLstIterint_deref(&it_at_idx) == 
            init_elems[ran_index]);

        Tundra_LnkLstint_free(&list);

        delete[] init_elems;
    }

}
TEST_END


TEST_MAIN(LnkLstTest)
