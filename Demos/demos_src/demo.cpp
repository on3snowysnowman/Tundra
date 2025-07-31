
#include "tundra/utils/Hash.hpp"
#include "tundra/utils/containers/String.hpp"

#include <stdio.h>

int main()
{   
    Tundra::Str::String<> str;

    Tundra::Str::init(&str, "Hello World!", 12);

    Tundra::uint64 hash = Tundra::hash<Tundra::Str::String<>>(&str);

    printf("%llu\n", hash);

    Tundra::Str::free(&str);

    return 0;
}