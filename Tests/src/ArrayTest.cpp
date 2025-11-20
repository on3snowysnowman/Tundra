/**
 * @file ArrayTest.cpp
 * @author Joel Height (On3SnowySnowman)
 * @brief Test file for testing the Array container.
 * @date 2025-11-17
 * 
 * @copyright Copyright (c) 2025
*/

#include <iostream>
#include <cassert>

#include "tundra/utils/containers/Array.hpp"


void test_make()
{
    std::cout << "test_make: ";
    Tundra::Arr::Array<int, 5> arr = Tundra::Arr::make<int>(1, 5, 4, 2, 3);

    assert(arr.data[0] == 1);
    assert(arr.data[1] == 5);
    assert(arr.data[2] == 4);
    assert(arr.data[3] == 2);
    assert(arr.data[4] == 3);
    std::cout << "Pass!\n";
}

void test_at()
{
    std::cout << "test_at: ";
    Tundra::Arr::Array<int, 5> arr = Tundra::Arr::make<int>(1, 5, 4, 2, 3);

    assert(Tundra::Arr::at_unchecked(arr, 0) == 1);
    assert(Tundra::Arr::at_unchecked(arr, 1) == 5);
    assert(Tundra::Arr::at_unchecked(arr, 2) == 4);
    assert(Tundra::Arr::at_unchecked(arr, 3) == 2);
    assert(Tundra::Arr::at_unchecked(arr, 4) == 3);
    assert(Tundra::Arr::at(arr, 0) == 1);
    assert(Tundra::Arr::at(arr, 1) == 5);
    assert(Tundra::Arr::at(arr, 2) == 4);
    assert(Tundra::Arr::at(arr, 3) == 2);
    assert(Tundra::Arr::at(arr, 4) == 3);

    // Const tests

    const Tundra::Arr::Array<int, 5> const_arr = 
        Tundra::Arr::make<int>(1, 5, 4, 2, 3);

    assert(Tundra::Arr::at_unchecked(const_arr, 0) == 1);
    assert(Tundra::Arr::at_unchecked(const_arr, 1) == 5);
    assert(Tundra::Arr::at_unchecked(const_arr, 2) == 4);
    assert(Tundra::Arr::at_unchecked(const_arr, 3) == 2);
    assert(Tundra::Arr::at_unchecked(const_arr, 4) == 3);
    assert(Tundra::Arr::at(const_arr, 0) == 1);
    assert(Tundra::Arr::at(const_arr, 1) == 5);
    assert(Tundra::Arr::at(const_arr, 2) == 4);
    assert(Tundra::Arr::at(const_arr, 3) == 2);
    assert(Tundra::Arr::at(const_arr, 4) == 3);

    std::cout << "Pass!\n";
}

void test_front_back()
{
    std::cout << "test_front_back: ";

    Tundra::Arr::Array<int, 5> arr = Tundra::Arr::make<int>(1, 5, 4, 2, 3);

    assert(Tundra::Arr::front(arr) == 1);
    assert(Tundra::Arr::back(arr) == 3);

    // Const tests

    const Tundra::Arr::Array<int, 5> const_arr = 
        Tundra::Arr::make<int>(1, 5, 4, 2, 3);

    assert(Tundra::Arr::front(const_arr) == 1);
    assert(Tundra::Arr::back(const_arr) == 3);

    std::cout << "Pass!\n";
}

void test_size()
{
    std::cout << "test_size: ";

    Tundra::Arr::Array<int, 5> arr = Tundra::Arr::make<int>(1, 5, 4, 2, 3);

    assert(Tundra::Arr::size(arr) == 5);

    // Const tests

    const Tundra::Arr::Array<int, 5> const_arr = 
        Tundra::Arr::make<int>(1, 5, 4, 2, 3);

    assert(Tundra::Arr::size(const_arr) == 5);

    std::cout << "Pass!\n";
}

void test_iterator()
{
    std::cout << "test_iterator: ";

    static constexpr int SIZE = 5;

    Tundra::Arr::Array<int, SIZE> arr = Tundra::Arr::make<int>(1, 5, 4, 2, 3);

    Tundra::Arr::Iterator<int> it = Tundra::Arr::begin(arr);

    assert(it.data == arr.data);
    assert(*it == 1);
    assert(it == Tundra::Arr::begin(arr));

    ++it;

    assert(it.data == arr.data + 1);
    assert(*it == 5);

    it = Tundra::Arr::end(arr);

    assert(it.data == arr.data + SIZE);

    --it;

    assert(it.data == arr.data + SIZE - 1);
    assert(*it == 3);

    // Const Tests

    const Tundra::Arr::Array<int, SIZE> arr_const = 
        Tundra::Arr::make<int>(1, 5, 4, 2, 3);

    Tundra::Arr::ConstIterator<int> it_c = Tundra::Arr::begin(arr_const);

    assert(it_c.data == arr_const.data);
    assert(*it_c == 1);
    assert(it_c == Tundra::Arr::begin(arr_const));

    ++it_c;

    assert(it_c.data == arr_const.data + 1);
    assert(*it_c == 5);

    it_c = Tundra::Arr::end(arr_const);

    assert(it_c.data == arr_const.data + SIZE);

    --it_c;

    assert(it_c.data == arr_const.data + SIZE - 1);
    assert(*it_c == 3);

    std::cout << "Pass!\n";
}

void run_all_tests()
{
    std::cout << "-- Running all tests --\n";
    test_make();
    test_at();
    test_front_back();
    test_size();
    test_iterator();
    std::cout << "--All tests passed --\n";
}

int main()
{
    run_all_tests();
    return 0;
}