
/**
 * Generic Demo file for testing Tundra Components.
*/

#ifndef TUNDRA_HASHTABLE_UINT8_UINT8
#define TUNDRA_HASHTABLE_UINT8_UINT8

#define TUNDRA_HSHTBL_KEYTYPE uint8_t
#define TUNDRA_HSHTBL_VALUETYPE uint8_t
#define TUNDRA_HSHTBL_NAME UInt8UInt8

#include "tundra/tundra_utils/HashTable.h"

#undef TUNDRA_HSHTBL_KEYTYPE
#undef TUNDRA_HSHTBL_VALUETYPE
#undef TUNDRA_HSHTBL_NAME
#endif

int main()
{
    Tundra_HashTableUInt8UInt8 table;

    Tundra_HshTblUInt8UInt8_init(&table);
    Tundra_HshTblUInt8UInt8_emplace(&table, (uint8_t[]){3}, (uint8_t[]){10});
    Tundra_HshTblUInt8UInt8_deconstruct(&table);
}
