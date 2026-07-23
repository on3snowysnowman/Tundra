/**
 * @file demo.c
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Simple demo file for arbitrary testing of the Tundra library.
 * @date 2025-12-03
 *
 * @copyright Copyright (c) 2025
 */

#include "tundra/Tundra.h"
#include "tundra/common/Hash.h"
#include "tundra/utils/ConsoleIO.h"

#define TUNDRA_KEY int
#define TUNDRA_KEYNAME int
#define TUNDRA_VAL int
#define TUNDRA_VALNAME int
#define TUNDRA_HASH_FUNC (*)(void)

#include "tundra/containers/DynamicStackU64.h"

#include "tundra/internal/container_templates/HashTable.h"

int main(void)
{
    if (Tundra_init() != 0) return -1;

    Tundra_HashTableintint tbl;
    Tundra_HshTblintint_init(&tbl);

    if (Tundra_shutdown() != 0) return -1;

    return 0;
}
