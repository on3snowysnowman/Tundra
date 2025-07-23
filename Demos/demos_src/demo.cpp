
#include "tundra/utils/containers/DynamicArray.hpp"

#include <stdio.h>

int main()
{
    Tundra::DynArr::DynamicArray<int> arr;

    static constexpr int elems[] = {1, 2, 3, 4, 5, 6, 7, 8};
    
    Tundra::DynArr::init(&arr, elems, 8);
    
    // Tundra::DynArr::add_multiple(&arr, elems, sizeof(elems) / sizeof(int));

    Tundra::DynArr::deconstruct(&arr);

    return 0;
}