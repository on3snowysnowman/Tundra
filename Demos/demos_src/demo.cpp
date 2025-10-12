
#include <iostream>

#include "tundra/utils/NumericLimits.hpp"
#include "tundra/utils/containers/String.hpp"


int main()
{
    Tundra::Str::String str;
    Tundra::Str::init(str, "Hello World!", 13);

    std::cout << Tundra::Str::data(str) << '\n';

    for(int i = 0; i < Tundra::Str::size(str); ++i)
    {
        std::cerr << Tundra::Str::at(str, i);
    }

    Tundra::Str::at(str, 13);

    std::cout << '\n';

    Tundra::Str::free(str);

    return 0;
}