
/**
 * Generic Demo file for testing components.
*/

#include <stdint.h>

#ifndef TUNDRA_LINKEDLISTUINT64
#define TUNDRA_LINKEDLISTUINT64
#define TUNDRA_LNKLST_TYPE uint64_t
#define TUNDRA_LNKLST_NAME UInt64
#include "tundra/tundra_utils/LinkedList.h"
#undef TUNDRA_LNKLST_TYPE
#undef TUNDRA_LNKLST_NAME
#endif


int main()
{
    Tundra_LinkedListUInt64 list;
    
    return 0;
}
