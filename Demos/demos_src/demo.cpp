
#include "tundra/utils/containers/DynamicArray.hpp"



int main()
{
    Tundra::DynArr::DynamicArray<int> arr;
    
    Tundra::DynArr::init(&arr);

    for(int i = 0; i < 10; ++i)
    {
        Tundra::DynArr::add(&arr, (int[]){i});
    }

    Tundra::DynArr::reserve_for(&arr, 12);

    Tundra::DynArr::deconstruct(&arr);
    return 0;
}