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


TEST_BEGIN(basic_init)
{
    Tundra_LinkedListint list;
    Tundra_LnkLstint_init(&list);

    assert(list.num_node == 0);
    assert(list.head_idx == TUNDRA_LNKLST_SENTINEL_IDX);
    assert(list.tail_idx == TUNDRA_LNKLST_SENTINEL_IDX);
    assert(list.cap_bytes == CALC_CAP_BYTES(TUNDRA_LNKLST_DEF_CAP));
    assert(list.cap == (list.cap_bytes / LIST_NODE_SIZE));
    assert(list.nodes);

    Tundra_LnkLstint_free(&list);
}
TEST_END


TEST_MAIN(LnkLstTest)
