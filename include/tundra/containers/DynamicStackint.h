#ifndef TUNDRA_DYNAMICSTACKint_H
#define TUNDRA_DYNAMICSTACKint_H

#include "tundra/internal/MacroHelper.h"

// Element handling flags for the template 
#define TUNDRA_NEEDS_CUSTOM_COPY 0
#define TUNDRA_NEEDS_CUSTOM_FREE 0
#define TUNDRA_NEEDS_CUSTOM_MOVE 0
#define TUNDRA_NEEDS_CUSTOM_INIT 0
#define TUNDRA_TYPE int
#define TUNDRA_TYPENAME int

// Define a DynamicArray of the given type if it does not exist.
#ifndef TUNDRA_DYNAMICARRAYint
#define TUNDRA_DYNAMICARRAYint
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
#undef TUNDRA_INIT_PARAMS
#undef TUNDRA_DECL_PARAM
#undef TUNDRA_CALL_PARAM

#endif // TUNDRA_DYNAMICSTACKint_H

