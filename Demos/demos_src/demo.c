
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

#include <stdio.h>


int main()
{
    Tundra_HashTableUInt8UInt8 table;

    Tundra_HshTblUInt8UInt8_init(&table);

    for(int i = 0; i < 15; ++i)
        Tundra_HshTblUInt8UInt8_add(&table, (uint8_t[]){i}, (uint8_t[]){i});

    for(uint64_t i = 0; i < table.top_capacity; ++i)
    {
        printf("%ld -> ", i);

        if(table.data[i].status == -2)
        {
            puts("No entry");
            continue;
        }

        printf("[%d, %d]", table.data[i].key, table.data[i].value);

        if(table.data[i].status == -1) 
        {
            puts("");
            continue;
        }

        printf(" -> Points to: %ld\n", table.data[i].status);
    }

    Tundra_HshTblUInt8UInt8_deconstruct(&table);
}
