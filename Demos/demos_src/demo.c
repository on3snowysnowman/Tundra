
/**
 * Generic Demo file for testing the Tundra Engine.
*/

#include <stdio.h>

#include <tundra/tundra_tools/String.h>


int main()
{
    Tundra_String string;

    Tundra_String_init(&string, 0);

    for(uint8_t i = 33; i < 128; ++i)
    {
        Tundra_String_add_char(&string, i);
    }

    for(int i = 0; i < string.chars.num_elements; ++i)
    {
        printf("%c", Tundra_String_at(&string, i));
    }

    printf("\n");

    Tundra_String_delete_string(&string);
}