
#include <iostream>

#include "tundra/utils/CoreTypes.hpp"
#include "tundra/internal/InternalMemAlloc.hpp"
#include "tundra/utils/memory/MemoryUtils.hpp"
#include "tundra/utils/Math.hpp"


void print_mem(const void *mem, Tundra::uint64 num_bytes)
{
    const Tundra::uint8 *mem_as_uint8_ptr = 
        reinterpret_cast<const Tundra::uint8*>(mem);

    for(int i = 0; i < num_bytes; ++i)
    {
        std::cout << int(mem_as_uint8_ptr[i]) << '\n';
    }
}


int main()
{   
    // Tundra::Internal::Mem::init();

    // static constexpr int SIZE = 12;

    // Tundra::uint8 *mem = 
    //     reinterpret_cast<Tundra::uint8*>(Tundra::Internal::Mem::malloc(SIZE));

    // Tundra::uint8 arr[SIZE] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
        
    // Tundra::copy_mem_fwd(arr, mem, SIZE);

    // print_mem(mem, SIZE);

    // std::cout << '\n';

    // for(int i = 0; i < SIZE; ++i)
    // {
    //     arr[i] *= arr[i];
    // }

    // Tundra::copy_mem_fwd(arr, mem, SIZE);

    // print_mem(mem, SIZE);

    // std::cout << '\n';

    // // Tundra::Internal::Mem::free(mem);

    // Tundra::Internal::Mem::init();

    // void *mem = Tundra::Internal::Mem::malloc(33);

    // Tundra::Internal::Mem::free(mem);

    std::cout << Tundra::pow2(5) << '\n';

    // std::cout << Tundra::pow2(0) << '\n';

    // std::cout << Tundra::pow(2, 3) << '\n';

    return 0;
}