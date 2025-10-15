
#include <iostream>

#include "tundra/utils/containers/DynamicArray.hpp"

void print_arr(const Tundra::DynArr::DynamicArray<int> &arr)
{
    for(int i = 0; i < Tundra::DynArr::size(arr); ++i)
    {
        std::cout << Tundra::DynArr::at(arr, i) << '\n';
    }
}


int main()
{
    Tundra::DynArr::DynamicArray<int> arr;
    Tundra::DynArr::init(arr, 10);

    for(int i = 0; i < 10; ++i)
    {
        Tundra::DynArr::add(arr, i);
    }

    std::cout << "Capacity: " << Tundra::DynArr::capacity(arr) << '\n';

    print_arr(arr);
    std::cout << '\n';

    Tundra::DynArr::insert(arr, 100, 4);
    print_arr(arr);

    Tundra::DynArr::free(arr);
    return 0;
}