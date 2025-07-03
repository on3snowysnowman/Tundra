
/**
 * Generic Demo file for testing Tundra Components.
*/

#ifndef TUNDRA_DYNAMICSTACKint
#define TUNDRA_DYNAMICSTACKint
#define TUNDRA_TYPE int
#define TUNDRA_NAME Int
#include "tundra/tundra_utils/DynamicStack.h"
#undef TUNDRA_TYPE
#undef TUNDRA_NAME

#endif

int main()
{
    Tundra_DynamicStackInt stack;

    Tundra_DynStkInt_init(&stack, 4);

    Tundra_DynStkInt_push(&stack, (int[]){3});

    Tundra_DynStkInt_reserve(&stack, 8);

    Tundra_DynStkInt_deconstruct(&stack);

    return 0;
}
