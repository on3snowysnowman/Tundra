
#include "tundra/utils/containers/String.hpp"

int main()
{
    Tundra::Str::String str;

    Tundra::Str::init(&str, "Hello World!", sizeof("Hello World!") - 1);

    puts(str.chars);

    Tundra::Str::deconstruct(&str);

    return 0;
}