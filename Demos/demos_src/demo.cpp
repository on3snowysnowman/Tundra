
#include "tundra/utils/containers/DynamicArray.hpp"

#include <cstdio>
#include <iostream>

void print_arr(const Tundra::DynArr::DynamicArray<int> &arr)
{
    for(int i = 0; i < Tundra::DynArr::size(arr); ++i)
    {
        std::cout << Tundra::DynArr::at(arr, i) << '\n';
    }

    std::cout << '\n';
}

int main()
{   
    Tundra::init_mem_sys();

    Tundra::DynArr::DynamicArray<int> arr;

    Tundra::DynArr::init(arr);

    for(int i = 0; i < 10; ++i)
    {
        Tundra::DynArr::add(arr, i);
    }

    print_arr(arr);

    std::cout << "Capacity: " << Tundra::DynArr::capacity(arr) << '\n';

    Tundra::DynArr::shrink_to_new_capacity(arr, 8);

    std::cout << "Capacity: " << Tundra::DynArr::capacity(arr) << '\n';

    print_arr(arr);

    Tundra::DynArr::erase(arr, 3);

    print_arr(arr);

    Tundra::DynArr::free(arr);

    return 0;
}