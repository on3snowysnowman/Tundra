
#include "tundra/utils/containers/DynamicArray.hpp"
#include "tundra/utils/NumericLimits.hpp"

#include <iostream>

void print_array(const Tundra::DynArr::DynamicArray<int> &arr)
{
    std::cout << "\nArray:\n";
    for(int i = 0; i < Tundra::DynArr::size(arr); ++i)
    {
        std::cout << *Tundra::DynArr::peek_unchecked(arr, i) << '\n';
    }

    std::cout << "Capacity: " << Tundra::DynArr::capacity(arr) << '\n';
}

int main()
{
    // Tundra::DynArr::DynamicArray<int> arr;

    // static constexpr int NUM_ELEMS = 4;

    // int buff[NUM_ELEMS] {1, 4, 9, 2};
    
    // Tundra::DynArr::init(arr, buff, NUM_ELEMS);

    // print_array(arr);

    // Tundra::DynArr::add(arr, 3);

    // print_array(arr);

    // Tundra::DynArr::free(arr);

    return 0;
}