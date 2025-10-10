
#include "tundra/utils/NumericLimits.hpp"
#include "tundra/utils/StringConversion.hpp"

#include <iostream>

// void print_array(const Tundra::DynArr::DynamicArray<int> &arr)
// {
//     std::cout << "\nArray:\n";
//     for(int i = 0; i < Tundra::DynArr::size(arr); ++i)
//     {
//         std::cout << *Tundra::DynArr::peek_unchecked(arr, i) << '\n';
//     }

//     std::cout << "Capacity: " << Tundra::DynArr::capacity(arr) << '\n';
// }

int main()
{
    Tundra::Str::String str;
    
    for(int i = 0; i < 100; ++i)
    {
        Tundra::Str::move(str, Tundra::num_to_str(i));

        std::cout << Tundra::Str::data(&str) << '\n';
    }

    Tundra::Str::free(&str);

    return 0;
}