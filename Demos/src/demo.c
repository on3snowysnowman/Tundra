/**
 * @file demo.c
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Simple demo file for arbitrary testing of the Tundra library.
 * @date 2025-12-03
 * 
 * @copyright Copyright (c) 2025
 * 
*/

#include "tundra/utils/BitUtils.h"

#include <stdio.h>

int main()
{
   printf("KIBIBYTE: %llu\n", TUNDRA_KIBIBYTE);
   printf("MEBIBYTE: %llu\n", TUNDRA_MEBIBYTE);
   printf("GIBIBYTE: %llu\n", TUNDRA_GIBIBYTE);

    return 0;
}