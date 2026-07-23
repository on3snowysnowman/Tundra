
#ifndef TUNDRA_DYNAMICSTACKU64_H
#define TUNDRA_DYNAMICSTACKU64_H

#include "tundra/internal/MacroHelper.h"

// Create dependant container
// -----------------------------------------------------------------------------
#ifndef TUNDRA_DYNAMICARRAYU64_H
#define TUNDRA_DYNAMICARRAYU64_H
#define TUNDRA_TYPE u64
#define TUNDRA_TYPENAME U64
#include "tundra/internal/container_templates/DynamicArray.h"
#undef TUNDRA_TYPE
#undef TUNDRA_TYPENAME
#endif

// Type flags for the template 
#define TUNDRA_NEEDS_CUSTOM_COPY 0
#define TUNDRA_NEEDS_CUSTOM_FREE 0
#define TUNDRA_NEEDS_CUSTOM_MOVE 0
#define TUNDRA_NEEDS_CUSTOM_INIT 0
#define TUNDRA_TYPE u64
#define TUNDRA_TYPENAME U64

// -----------------------------------------------------------------------------
// Create specialization for the given type
#include "tundra/internal/container_templates/DynamicStack.h"

// Clean up
#undef TUNDRA_TYPE
#undef TUNDRA_TYPENAME
#undef TUNDRA_NEEDS_CUSTOM_COPY
#undef TUNDRA_NEEDS_CUSTOM_FREE
#undef TUNDRA_NEEDS_CUSTOM_MOVE
#undef TUNDRA_NEEDS_CUSTOM_INIT
#endif // TUNDRA_DYNAMICSTACKU64_H
