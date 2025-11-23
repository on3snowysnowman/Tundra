/**
 * @file DynArrTest.cpp
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Test file for testing the DynamicArray container.
 * @date 2025-11-21
 * 
 * @copyright Copyright (c) 2025
*/

#include <iostream>
#include <cassert>

#include "tundra/utils/containers/DynamicArray.hpp"
#include "tundra/Tundra.hpp"


void test_init()
{
    std::cout << "test_init: ";
    std::cout.flush();

    // Default init method
    Tundra::DynArr::DynamicArray<int> arr_one;
    Tundra::DynArr::init(arr_one);

    assert(arr_one.cap == Tundra::DynArr::Internal::DEF_CAP);
    assert(arr_one.num_elem == 0);
    assert(arr_one.data);

    // Specified capacity init method
    Tundra::DynArr::DynamicArray<int> arr_two;
    Tundra::DynArr::init(arr_two, 10);

    assert(arr_two.cap == 10);
    assert(arr_two.num_elem == 0);
    assert(arr_two.data);

    // Initial elements init method
    constexpr int NUM_INIT_ELEM = 5;
    constexpr int elems[NUM_INIT_ELEM] = {1, 2, 3, 4, 5};

    Tundra::DynArr::DynamicArray<int> arr_thr;

    // Test without strict alloc
    Tundra::DynArr::init(arr_thr, elems, NUM_INIT_ELEM);

    assert(arr_thr.cap == 8);
    assert(arr_thr.num_elem == NUM_INIT_ELEM);
    assert(arr_thr.data);

    for(int i = 0; i < NUM_INIT_ELEM; ++i)
    {
        assert(arr_thr.data[i] == elems[i]);
    }

    Tundra::DynArr::free(arr_thr);

    // Test with strict alloc
    Tundra::DynArr::init(arr_thr, elems, NUM_INIT_ELEM, true);

    assert(arr_thr.cap == NUM_INIT_ELEM);
    assert(arr_thr.num_elem == NUM_INIT_ELEM);
    assert(arr_thr.data);

    for(int i = 0; i < NUM_INIT_ELEM; ++i)
    {
        assert(arr_thr.data[i] == elems[i]);
    }

    Tundra::DynArr::free(arr_one);
    Tundra::DynArr::free(arr_two);
    Tundra::DynArr::free(arr_thr);

    std::cout << "Pass!" << std::endl;
}

void test_free()
{
    std::cout << "test_free: ";
    std::cout.flush();

    Tundra::DynArr::DynamicArray<int> arr;
    Tundra::DynArr::init(arr);
    Tundra::DynArr::free(arr);

    assert(arr.data == nullptr);

    std::cout << "Pass!" << std::endl;
}

void test_copy()
{
    std::cout << "test_copy: ";
    std::cout.flush();

    Tundra::DynArr::DynamicArray<int> src;
    Tundra::DynArr::DynamicArray<int> dst;
    
    constexpr int NUM_INIT_ELEM = 5;
    constexpr int elems[NUM_INIT_ELEM] = {1, 2, 3, 4, 5};

    Tundra::DynArr::init(src, elems, NUM_INIT_ELEM);

    // dst purposely left uninitialized.
    Tundra::DynArr::copy(src, dst);

    assert(dst.cap == src.cap);
    assert(dst.num_elem == src.num_elem);
    assert(dst.data != src.data);

    for(int i = 0; i < NUM_INIT_ELEM; ++i)
    {
        assert(dst.data[i] == elems[i]);
    }

    Tundra::DynArr::free(src);
    Tundra::DynArr::free(dst);

    std::cout << "Pass!" << std::endl;
}

void test_move()
{
    std::cout << "test_move: ";
    std::cout.flush();

    Tundra::DynArr::DynamicArray<int> src;
    Tundra::DynArr::DynamicArray<int> dst;
    
    constexpr int NUM_INIT_ELEM = 5;
    constexpr int CAP_SIZE = 8;
    constexpr int elems[NUM_INIT_ELEM] = {1, 2, 3, 4, 5};

    Tundra::DynArr::init(src, elems, NUM_INIT_ELEM);

    // dst purposely left uninitialized.
    Tundra::DynArr::move(src, dst);

    assert(dst.cap == CAP_SIZE);
    assert(dst.num_elem == NUM_INIT_ELEM);
    assert(src.data == nullptr);

    for(int i = 0; i < NUM_INIT_ELEM; ++i)
    {
        assert(dst.data[i] == elems[i]);
    }

    Tundra::DynArr::free(dst);

    std::cout << "Pass!" << std::endl;
}

void test_clear()
{
    std::cout << "test_clear: ";
    std::cout.flush();

    constexpr int NUM_INIT_ELEM = 5;
    constexpr int elems[NUM_INIT_ELEM] = {1, 2, 3, 4, 5};

    Tundra::DynArr::DynamicArray<int> arr;

    Tundra::DynArr::init(arr, elems, NUM_INIT_ELEM);

    Tundra::DynArr::clear(arr);

    assert(arr.num_elem == 0);
    assert(arr.data);
    assert(arr.cap != 0);

    Tundra::DynArr::free(arr);
    
    std::cout << "Pass!" << std::endl; 
}

void test_add()
{
    std::cout << "test_add: ";
    std::cout.flush();

    Tundra::DynArr::DynamicArray<int> arr;
    Tundra::DynArr::init(arr, 2);

    Tundra::DynArr::add(arr, 1);
    
    assert(arr.data[0] == 1);
    assert(arr.cap == 2);

    Tundra::DynArr::add(arr, 2);

    assert(arr.data[1] == 2);
    assert(arr.cap == 2);

    Tundra::DynArr::add(arr, 3);

    assert(arr.data != nullptr);
    assert(arr.data[2] == 3);
    assert(arr.cap == 4);

    Tundra::DynArr::free(arr);

    std::cout << "Pass!" << std::endl;
}

void test_add_multiple()
{
    std::cout << "test_add_multiple: ";
    std::cout.flush();

    constexpr int NUM_INIT_ELEM = 5;
    constexpr int STARTING_CAP = 8;
    constexpr int elems[NUM_INIT_ELEM] = {1, 2, 3, 4, 5};

    Tundra::DynArr::DynamicArray<int> arr;

    Tundra::DynArr::init(arr, elems, NUM_INIT_ELEM);

    assert(arr.cap == STARTING_CAP);
    assert(arr.num_elem == NUM_INIT_ELEM);

    Tundra::DynArr::add_multiple(arr, elems, NUM_INIT_ELEM);

    // Ensure the Array has expanded.
    assert(arr.cap == (STARTING_CAP * 2));
    assert(arr.num_elem == (NUM_INIT_ELEM * 2));

    // Check that initial elements were copied over properly, since the Array 
    // should've needed to expand.
    for(int i = 0; i < NUM_INIT_ELEM; ++i)
    {
        assert(arr.data[i] == elems[i]);
    }

    // Check that the additional elements were added correctly.
    for(int i = NUM_INIT_ELEM; i < NUM_INIT_ELEM * 2; ++i)
    {
        assert(arr.data[i] == elems[i - NUM_INIT_ELEM]);
    }

    Tundra::DynArr::free(arr);

    std::cout << "Pass!" << std::endl;
}

void test_insert()
{
    std::cout << "test_insert: ";
    std::cout.flush();

    constexpr int NUM_INIT_ELEM = 3;
    constexpr int STARTING_CAP = 4;
    constexpr int elems[NUM_INIT_ELEM] = {1, 2, 3};

    Tundra::DynArr::DynamicArray<int> arr;

    Tundra::DynArr::init(arr, elems, NUM_INIT_ELEM);

    assert(arr.cap == STARTING_CAP);
    assert(arr.num_elem == NUM_INIT_ELEM);

    Tundra::DynArr::insert(arr, 8, 1);

    assert(arr.num_elem == NUM_INIT_ELEM + 1);

    assert(arr.data[0] == 1);
    assert(arr.data[1] == 8);
    assert(arr.data[2] == 2);
    assert(arr.data[3] == 3);

    // Insert at one past the last element. Array should need to expand for 
    // this.
    Tundra::DynArr::insert(arr, 9, arr.num_elem);

    assert(arr.num_elem == NUM_INIT_ELEM + 2);

    assert(arr.data[0] == 1);
    assert(arr.data[1] == 8);
    assert(arr.data[2] == 2);
    assert(arr.data[3] == 3);
    assert(arr.data[4] == 9);

    assert(arr.cap == 2 * STARTING_CAP);

    Tundra::DynArr::free(arr);

    std::cout << "Pass!" << std::endl;
}

void test_resize()
{
    std::cout << "test_resize: ";
    std::cout.flush();

    Tundra::DynArr::DynamicArray<int> arr;
    Tundra::DynArr::init(arr);

    Tundra::DynArr::add(arr, 1);
    Tundra::DynArr::add(arr, 2);
    Tundra::DynArr::add(arr, 3);

    assert(arr.num_elem == 3);
    assert(arr.cap == 4);

    Tundra::DynArr::resize(arr, 9);

    assert(arr.num_elem == 9);
    assert(arr.cap == 16);

    Tundra::DynArr::free(arr);

    std::cout << "Pass!" << std::endl;
}

void test_shrink()
{
    std::cout << "test_shrink: ";
    std::cout.flush();

    constexpr int NUM_INIT_ELEM = 5;
    constexpr int elems[NUM_INIT_ELEM] = {1, 2, 3, 4, 5};

    Tundra::DynArr::DynamicArray<int> arr;
    Tundra::DynArr::init(arr, elems, NUM_INIT_ELEM);

    assert(arr.num_elem == NUM_INIT_ELEM);
    assert(arr.cap == 8);

    Tundra::DynArr::shrink_to_fit(arr);

    assert(arr.cap == NUM_INIT_ELEM);
    assert(arr.num_elem == NUM_INIT_ELEM);

    Tundra::DynArr::shrink_to_new_cap(arr, 2);

    assert(arr.cap == 2);
    assert(arr.num_elem == 2);

    Tundra::DynArr::free(arr);
    std::cout << "Pass!" << std::endl;
}

void test_erase()
{
    std::cout << "test_erase: ";
    std::cout.flush();

    constexpr int NUM_INIT_ELEM = 5;
    constexpr int elems[NUM_INIT_ELEM] = {1, 2, 3, 4, 5};

    Tundra::DynArr::DynamicArray<int> arr;
    Tundra::DynArr::init(arr, elems, NUM_INIT_ELEM);

    Tundra::DynArr::erase(arr, 2);

    assert(arr.num_elem == NUM_INIT_ELEM - 1);

    assert(arr.data[0] == 1);
    assert(arr.data[1] == 2);
    assert(arr.data[2] == 4);
    assert(arr.data[3] == 5);
    
    Tundra::DynArr::free(arr);

    std::cout << "Pass!" << std::endl;
}

void test_front_back()
{
    std::cout << "test_front_back: ";
    std::cout.flush();

    constexpr int NUM_INIT_ELEM = 5;
    constexpr int elems[NUM_INIT_ELEM] = {1, 2, 3, 4, 5};

    Tundra::DynArr::DynamicArray<int> arr;
    Tundra::DynArr::init(arr, elems, NUM_INIT_ELEM);

    assert(Tundra::DynArr::front(arr) == 1);
    assert(Tundra::DynArr::back(arr) == 5);

    // Const Tests

   const Tundra::DynArr::DynamicArray<int> &arr_ref = arr;

   assert(Tundra::DynArr::front(arr_ref) == 1);
   assert(Tundra::DynArr::back(arr_ref) == 5);

    Tundra::DynArr::free(arr);

    std::cout << "Pass!" << std::endl;
}

void test_at()
{
    std::cout << "test_at: ";
    std::cout.flush();

    constexpr int NUM_INIT_ELEM = 5;
    constexpr int elems[NUM_INIT_ELEM] = {1, 2, 3, 4, 5};

    Tundra::DynArr::DynamicArray<int> arr;
    Tundra::DynArr::init(arr, elems, NUM_INIT_ELEM);

    assert(Tundra::DynArr::at_nocheck(arr, 2) == 3);
    assert(Tundra::DynArr::at(arr, 2) == 3);

    // Const Tests

    const Tundra::DynArr::DynamicArray<int> &arr_ref = arr;

    assert(Tundra::DynArr::at_nocheck(arr_ref, 2) == 3);
    assert(Tundra::DynArr::at(arr_ref, 2) == 3);

    Tundra::DynArr::free(arr);

    std::cout << "Pass!" << std::endl;
}

void test_iterator()
{
    std::cout << "test_iterator: ";
    std::cout.flush();

    constexpr int NUM_INIT_ELEM = 5;
    constexpr int elems[NUM_INIT_ELEM] = {1, 2, 3, 4, 5};

    Tundra::DynArr::DynamicArray<int> arr;
    Tundra::DynArr::init(arr, elems, NUM_INIT_ELEM);

    const Tundra::DynArr::DynamicArray<int> &arr_ref = arr;

    Tundra::DynArr::Iterator<int> beg = Tundra::DynArr::begin(arr);
    Tundra::DynArr::ConstIterator<int> beg_const = 
        Tundra::DynArr::begin(arr_ref);
    Tundra::DynArr::Iterator<int> end = Tundra::DynArr::end(arr);
    Tundra::DynArr::ConstIterator<int> end_const = 
        Tundra::DynArr::end(arr_ref);

    assert(beg.datum == arr.data);
    assert(end.datum == arr.data + arr.cap);
    assert(beg_const.datum == arr.data);
    assert(end_const.datum == arr.data + arr.cap);

    for(int i = 0; i < NUM_INIT_ELEM; ++i)
    {
        assert(*beg == arr.data[i]);
        assert(*beg_const == arr.data[i]);
        ++beg;
        ++beg_const;
    }

    assert(beg == end);
    assert(beg_const == end_const);

    Tundra::DynArr::free(arr);

    std::cout << "Pass!" << std::endl;
}

void run_all_tests()
{
    std::cout << " -- Running all tests --\n";
    test_init();
    test_free();
    test_copy();
    test_move();
    test_clear();
    test_add();
    test_add_multiple();
    test_insert();
    test_resize();
    test_shrink();
    test_erase();
    test_front_back();
    test_at();
    std::cout << " -- All tests passed --\n";
}

int main()
{
    assert(Tundra::init_tundra() == 0);

    run_all_tests();
    return 0;
}