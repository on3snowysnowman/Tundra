/**
 * @file StringTest.cpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Test file for testing the String container.
 * @date 2025-12-06
 * 
 * @copyright Copyright (c) 2025
*/

#include <cassert>

#include "../TestingHelper.hpp"
#include "tundra/Tundra.h"
#include "tundra/containers/String.h"


TEST_BEGIN(init)
{
    // Default init method
    Tundra_String str_one;
    Tundra_Str_init(&str_one);

    assert(str_one.cap == TUNDRA_STR_DEF_CAP);
    assert(str_one.num_char == 1); // 1 for null terminator
    assert(str_one.chars);

    // Specified capacity init method
    Tundra_String str_two;
    Tundra_Str_init_w_cap(&str_two, 10);

    assert(str_two.cap == 10);
    assert(str_two.num_char == 1); // 1 for null terminator
    assert(str_two.chars);

    // Initial chars init method
    const char init_chars[] = {'H', 'e', 'l', 'l', 'o'};
    const uint64 NUM_INIT_CHAR = 5;

    Tundra_String str_thr;

    // Test without strict alloc
    Tundra_Str_init_w_chars(&str_thr, init_chars, NUM_INIT_CHAR, false);

    assert(str_thr.cap >= NUM_INIT_CHAR + 1); // +1 for null terminator
    assert(str_thr.num_char == NUM_INIT_CHAR + 1); // +1 for null terminator
    assert(str_thr.chars);

    for (uint64 i = 0; i < NUM_INIT_CHAR; ++i)
    {
        assert(str_thr.chars[i] == init_chars[i]);
    }
    assert(str_thr.chars[NUM_INIT_CHAR] == '\0');

    Tundra_Str_free(&str_thr);

    // Test with strict alloc
    Tundra_Str_init_w_chars(&str_thr, init_chars, NUM_INIT_CHAR, true);

    assert(str_thr.cap == NUM_INIT_CHAR + 1); // +1 for null terminator
    assert(str_thr.num_char == NUM_INIT_CHAR + 1); // +1 for null terminator
    assert(str_thr.chars);

    for (uint64 i = 0; i < NUM_INIT_CHAR; ++i)
    {
        assert(str_thr.chars[i] == init_chars[i]);
    }
    assert(str_thr.chars[NUM_INIT_CHAR] == '\0');

    Tundra_Str_free(&str_thr);
}
TEST_END

TEST_BEGIN(free)
{
    Tundra_String str;
    Tundra_Str_init(&str);

    Tundra_Str_free(&str);

    // Freeing again should be safe.
    Tundra_Str_free(&str);
}
TEST_END

int main()
{
    std::cout << "StringTest: \n";

    assert(Tundra_init() == 0);

    run_all_tests();

    std::cout << "\n";
    return 0;
}
