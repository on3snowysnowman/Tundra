#ifndef TUNDRA_DYNAMICSTACKint_H
#define TUNDRA_DYNAMICSTACKint_H

#include "tundra/internal/MacroHelper.h"

// Type flags for the template 
#define TUNDRA_NEEDS_CUSTOM_COPY 0
#define TUNDRA_NEEDS_CUSTOM_FREE 0
#define TUNDRA_NEEDS_CUSTOM_MOVE 0
#define TUNDRA_NEEDS_CUSTOM_INIT 0
#define TUNDRA_TYPE int
#define TUNDRA_TYPENAME int

// -----------------------------------------------------------------------------

// Create specialization for the DynamicArray which is required by the 
// DynamicStack
#ifndef TUNDRA_DYNAMICARRAYint_H
#define TUNDRA_DYNAMICARRAYint_H
#include "tundra/internal/container_templates/DynamicArray.h"
#endif

// Create specialization for the given type
#include "tundra/internal/container_templates/DynamicStack.h"

// Clean up
#undef TUNDRA_TYPE
#undef TUNDRA_TYPENAME
#undef TUNDRA_NEEDS_CUSTOM_COPY
#undef TUNDRA_NEEDS_CUSTOM_FREE
#undef TUNDRA_NEEDS_CUSTOM_MOVE
#undef TUNDRA_NEEDS_CUSTOM_INIT

#endif // TUNDRA_DYNAMICSTACKint_H

