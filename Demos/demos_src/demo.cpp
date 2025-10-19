
#include <iostream>

#include "tundra/internal/InternalMemAlloc.hpp"

int main()
{
    Tundra::Internal::Mem::free(reinterpret_cast<void*>(0x1));
    
    Tundra::Internal::Mem::init_mem_arena();

    Tundra::uint8 *mem = (Tundra::uint8*)Tundra::Internal::Mem::malloc(13);

    const char *message = "Hello World!";

    for(int i = 0; i < 13; ++i)
    {
        mem[i] = message[i];
    }

    std::cout << mem << '\n';

}