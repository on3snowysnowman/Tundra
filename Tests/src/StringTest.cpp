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

TEST_BEGIN(copy)
{
    const char init_chars[] = {'H', 'e', 'l', 'l', 'o'};
    const uint64 NUM_INIT_CHAR = 5;

    Tundra_String src;
    Tundra_Str_init_w_chars(&src, init_chars, NUM_INIT_CHAR, false);

    Tundra_String dst;
    Tundra_Str_init(&dst);

    Tundra_Str_copy(&src, &dst);

    assert(dst.num_char == src.num_char);
    assert(dst.cap == src.cap);

    for (uint64 i = 0; i < src.num_char; ++i)
    {
        assert(dst.chars[i] == src.chars[i]);
    }

    Tundra_Str_free(&src);
    Tundra_Str_free(&dst);
}
TEST_END

TEST_BEGIN(move)
{
    const char init_chars[] = {'H', 'e', 'l', 'l', 'o'};
    const uint64 NUM_INIT_CHAR = 5;

    Tundra_String src;
    Tundra_Str_init_w_chars(&src, init_chars, NUM_INIT_CHAR, false);

    Tundra_String dst;
    Tundra_Str_init(&dst);

    Tundra_Str_move(&src, &dst);

    assert(dst.num_char == NUM_INIT_CHAR + 1); // +1 for null terminator
    assert(dst.cap >= NUM_INIT_CHAR + 1);

    for (uint64 i = 0; i < NUM_INIT_CHAR; ++i)
    {
        assert(dst.chars[i] == init_chars[i]);
    }
    assert(dst.chars[NUM_INIT_CHAR] == '\0');

    assert(src.chars == NULL);

    Tundra_Str_free(&dst);
}
TEST_END

TEST_BEGIN(clear)
{
    const char init_chars[] = {'H', 'e', 'l', 'l', 'o'};
    const uint64 NUM_INIT_CHAR = 5;

    Tundra_String str;
    Tundra_Str_init_w_chars(&str, init_chars, NUM_INIT_CHAR, false);

    Tundra_Str_clear(&str);

    assert(str.num_char == 1); // 1 for null terminator
    assert(str.cap >= NUM_INIT_CHAR + 1);
    assert(str.chars);

    Tundra_Str_free(&str);
}
TEST_END

TEST_BEGIN(add)
{
    Tundra_String str;
    Tundra_Str_init(&str);

    const char chars_to_add[] = {'H', 'e', 'l', 'l', 'o'};
    const uint64 NUM_CHARS_TO_ADD = 5;

    for (uint64 i = 0; i < NUM_CHARS_TO_ADD; ++i)
    {
        Tundra_Str_add(&str, chars_to_add[i]);
    }

    assert(str.num_char == NUM_CHARS_TO_ADD + 1); // +1 for null terminator

    for (uint64 i = 0; i < NUM_CHARS_TO_ADD; ++i)
    {
        assert(str.chars[i] == chars_to_add[i]);
    }
    assert(str.chars[NUM_CHARS_TO_ADD] == '\0');

    Tundra_Str_free(&str);
}
TEST_END

TEST_BEGIN(add_multiple)
{
    Tundra_String str;
    Tundra_Str_init(&str);

    const char chars_to_add[] = {'H', 'e', 'l', 'l', 'o'};
    const uint64 NUM_CHARS_TO_ADD = 5;

    Tundra_Str_add_multiple(&str, chars_to_add, NUM_CHARS_TO_ADD);

    assert(str.num_char == NUM_CHARS_TO_ADD + 1); // +1 for null terminator

    for (uint64 i = 0; i < NUM_CHARS_TO_ADD; ++i)
    {
        assert(str.chars[i] == chars_to_add[i]);
    }
    assert(str.chars[NUM_CHARS_TO_ADD] == '\0');

    Tundra_Str_free(&str);
}
TEST_END

TEST_BEGIN(insert)
{
    Tundra_String str;
    Tundra_Str_init_w_chars(&str, "Helo", 4, false);

    Tundra_Str_insert(&str, 'l', 2);

    const char expected_chars[] = {'H', 'e', 'l', 'l', 'o'};
    const uint64 NUM_EXPECTED_CHARS = 5;

    assert(str.num_char == NUM_EXPECTED_CHARS + 1); // +1 for null terminator

    for (uint64 i = 0; i < NUM_EXPECTED_CHARS; ++i)
    {
        assert(str.chars[i] == expected_chars[i]);
    }
    assert(str.chars[NUM_EXPECTED_CHARS] == '\0');

    Tundra_Str_free(&str);
}
TEST_END

TEST_BEGIN(resize)
{
    Tundra_String str;
    Tundra_Str_init_w_chars(&str, "Hello", 5, false);

    Tundra_Str_resize(&str, 10);

    assert(str.cap >= 10);
    assert(str.num_char == 11); // 10 chars + 1 null terminator

    const char expected_chars[] = {'H', 'e', 'l', 'l', 'o'};
    const uint64 NUM_EXPECTED_CHARS = 5;

    for (uint64 i = 0; i < NUM_EXPECTED_CHARS; ++i)
    {
        assert(str.chars[i] == expected_chars[i]);
    }
    assert(str.chars[NUM_EXPECTED_CHARS] == '\0');

    Tundra_Str_free(&str);
}
TEST_END

TEST_BEGIN(reserve)
{
    Tundra_String str;
    Tundra_Str_init_w_chars(&str, "Hello", 5, false);

    const uint64 OLD_CAP = str.cap;

    Tundra_Str_reserve(&str, 20);

    assert(str.cap >= OLD_CAP + 20);
    assert(str.num_char == 6); // 5 chars + 1 null terminator

    const char expected_chars[] = {'H', 'e', 'l', 'l', 'o'};
    const uint64 NUM_EXPECTED_CHARS = 5;

    for (uint64 i = 0; i < NUM_EXPECTED_CHARS; ++i)
    {
        assert(str.chars[i] == expected_chars[i]);
    }
    assert(str.chars[NUM_EXPECTED_CHARS] == '\0');

    Tundra_Str_free(&str);
}
TEST_END

TEST_BEGIN(shrink)
{
    Tundra_String str;
    Tundra_Str_init_w_chars(&str, "Hello, World!", 13, false);

    Tundra_Str_shrink_to_fit(&str);

    assert(str.cap == str.num_char);

    Tundra_Str_shrink_to_new_cap(&str, 5);

    assert(str.cap == 6); // 5 chars + 1 null terminator
    assert(str.num_char == 6); // "Hello" + null terminator

    const char expected_chars[] = {'H', 'e', 'l', 'l', 'o'};
    const uint64 NUM_EXPECTED_CHARS = 5;

    for (uint64 i = 0; i < NUM_EXPECTED_CHARS; ++i)
    {
        assert(str.chars[i] == expected_chars[i]);
    }

    assert(str.chars[NUM_EXPECTED_CHARS] == '\0');

    Tundra_Str_free(&str);
}
TEST_END

TEST_BEGIN(erase)
{
    Tundra_String str;
    Tundra_Str_init_w_chars(&str, "Hello, World!", 13, false);

    Tundra_Str_erase(&str, 5); // Erase ','

    const char expected_chars[] = {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!'};
    const uint64 NUM_EXPECTED_CHARS = 12;

    assert(str.num_char == NUM_EXPECTED_CHARS + 1); // +1 for null terminator

    for (uint64 i = 0; i < NUM_EXPECTED_CHARS; ++i)
    {
        assert(str.chars[i] == expected_chars[i]);
    }
    assert(str.chars[NUM_EXPECTED_CHARS] == '\0');

    Tundra_Str_free(&str);
}
TEST_END

TEST_BEGIN(front_back_at)
{
    Tundra_String str;
    Tundra_Str_init_w_chars(&str, "Hello", 5, false);

    assert(*Tundra_Str_front(&str) == 'H');
    assert(*Tundra_Str_back(&str) == 'o');
    assert(*Tundra_Str_at(&str, 1) == 'e');

    assert(*(Tundra_Str_front_cst(&str)) == 'H');
    assert(*(Tundra_Str_back_cst(&str)) == 'o');
    assert(*(Tundra_Str_at_cst(&str, 1)) == 'e');

    Tundra_Str_free(&str);
}
TEST_END

TEST_BEGIN(data)
{
    Tundra_String str;
    Tundra_Str_init_w_chars(&str, "Hello", 5, false);

    const char* data = Tundra_Str_data(&str);

    for (uint64 i = 0; i < 5; ++i)
    {
        assert(data[i] == str.chars[i]);
    }
    assert(data[5] == '\0');

    Tundra_Str_free(&str);
}
TEST_END

TEST_BEGIN(size_capacity)
{
    Tundra_String str;
    Tundra_Str_init_w_chars(&str, "Hello", 5, false);

    assert(Tundra_Str_size(&str) == 5);
    assert(Tundra_Str_capacity(&str) >= 5);

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
