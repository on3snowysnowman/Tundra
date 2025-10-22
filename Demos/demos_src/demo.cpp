
#include <iostream>

#include "tundra/internal/InternalMemAlloc.hpp"

int main()
{   
   Tundra::Internal::Mem::init();

    static constexpr int SIZE = 12;

   void *mem = Tundra::Internal::Mem::malloc(SIZE);

    std::cout << "Got mem: " << mem << '\n';

   Tundra::Internal::Mem::free(mem);

    mem = Tundra::Internal::Mem::malloc(SIZE + SIZE);

    std::cout << "Got mem: " << mem << '\n';

    void *mem_two = Tundra::Internal::Mem::malloc(SIZE);

    std::cout << "Got mem: " << mem_two << '\n';
    
    Tundra::Internal::Mem::free(mem_two);

    return 0;
}