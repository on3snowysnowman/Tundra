
#include "tundra/utils/containers/String.hpp"



int main()
{
    Tundra::Str::String str;

    Tundra::Str::init(&str);

    Tundra::Str::add_chars(&str, "Hello World!", 12);

    Tundra::Str::deconstruct(&str);

    return 0;
}