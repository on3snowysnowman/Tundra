
#include "tundra/internal/MacroHelper.h"

#ifndef TUNDRA_DYNAMICARRAYINT_H
#define TUNDRA_DYNAMICARRAYINT_H

#define TYPE int
#define NAME TUNDRA_CONCAT(Tundra_DynamicArray, TYPE)
#define FUNC_NAME(name) TUNDRA_CONCAT3(Tundra_DynArr, TYPE, _##name)
#define INT_FUNC_NAME(name) TUNDRA_CONCAT3(InTundra_DynArr, TYPE, _##name)
#include "tundra/internal/DynamicArrayTemplate.h"
#undef TYPE
#undef NAME
#undef FUNC_NAME
#undef INT_FUNC_NAME

#endif
