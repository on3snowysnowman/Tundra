
/**
 * Generic Demo file for testing Tundra Components.
*/


#include "tundra/tundra_utils/String.h"


int main()
{
    Tundra_String str;

    Tundra_Str_init(&str);

    Tundra_Str_add_char(&str, 'a');

    Tundra_Str_deconstruct(&str);

    return 0;
}
