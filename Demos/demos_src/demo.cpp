
#include <iostream>

#include "tundra/internal/MemAllocHandler.hpp"

int main()
{   
    Tundra::Internal::Mem::init();

   void *mem = Tundra::Internal::Mem::get_mem_from_os();

//    Tundra::Internal::Mem::release_mem_to_os(mem, 10);

    return 0;
}