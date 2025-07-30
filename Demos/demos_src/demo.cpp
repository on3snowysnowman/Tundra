
#include <stdio.h>
#include <string>

#include "tundra/utils/containers/String.hpp"

int main()
{   
    for(int i = 0; i < 100000; ++i)
    {

        Tundra::Str::String str;
    
        Tundra::Str::init(&str, "Hello World!", 12);
    
        Tundra::Str::add_char(&str, 'j');
        Tundra::Str::add_chars(&str, "This is chars!!", 15);
    
        Tundra::Str::free(&str);
    }

    return 0;
}