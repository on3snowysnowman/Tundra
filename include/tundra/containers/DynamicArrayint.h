#ifndef TUNDRA_DYNAMICARRAYint_H
#define TUNDRA_DYNAMICARRAYint_H

#include "tundra/internal/MacroHelper.h"

// Element handling flags for the template 
#define TUNDRA_NEEDS_CUSTOM_COPY 0
#define TUNDRA_NEEDS_CUSTOM_FREE 0
#define TUNDRA_NEEDS_CUSTOM_MOVE 0
#define TUNDRA_NEEDS_CUSTOM_INIT 0
#define TUNDRA_TYPE int
#define TUNDRA_TYPENAME int

// Create specialization for the given type
#include "tundra/internal/container_templates/DynamicArray.h"

// Clean up
#undef TUNDRA_TYPE
#undef TUNDRA_NEEDS_CUSTOM_COPY
#undef TUNDRA_NEEDS_CUSTOM_FREE
#undef TUNDRA_NEEDS_CUSTOM_MOVE
#undef TUNDRA_NEEDS_CUSTOM_INIT
#undef TUNDRA_INIT_PARAMS
#undef TUNDRA_DECL_PARAM
#undef TUNDRA_CALL_PARAM

#endif // TUNDRA_DYNAMICARRAYint_H

