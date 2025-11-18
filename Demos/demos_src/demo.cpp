
#include "tundra/utils/memory/MemAlloc.hpp"
#include "tundra/utils/containers/String.hpp"

#include <iostream>

int main()
{   
    Tundra::init_mem_sys();

    Tundra::Str::String str;
    Tundra::Str::init(str, "STRING!", 7);

    return 0;
}