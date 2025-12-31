#ifndef TUNDRA_LINKEDLISTint_H
#define TUNDRA_LINKEDLISTint_H

#include "tundra/internal/MacroHelper.h"

// Establish depended Tundra_DynamicStacku64 container if it does not exist.
#ifndef TUNDRA_DYNAMICSTACKu64_H
#define TUNDRA_DYNAMICSTACKu64_H

#define TUNDRA_NEEDS_CUSTOM_COPY 0
#define TUNDRA_NEEDS_CUSTOM_FREE 0
#define TUNDRA_NEEDS_CUSTOM_MOVE 0
#define TUNDRA_TYPE uint64
#define TUNDRA_TYPENAME u64

// Create specialization for the DynamicArray which is required by the 
// DynamicStack
#ifndef TUNDRA_DYNAMICARRAYu64_H
#define TUNDRA_DYNAMICARRAYu64_H
#include "tundra/internal/container_templates/DynamicArray.h"
#endif

// Create specialization for the DynamicStack
#include "tundra/internal/container_templates/DynamicStack.h"

#undef TUNDRA_NEEDS_CUSTOM_COPY
#undef TUNDRA_NEEDS_CUSTOM_FREE
#undef TUNDRA_NEEDS_CUSTOM_MOVE
#undef TUNDRA_TYPE
#undef TUNDRA_TYPENAME

#endif

// Type flags for the template 
#define TUNDRA_NEEDS_CUSTOM_COPY 0
#define TUNDRA_NEEDS_CUSTOM_FREE 0
#define TUNDRA_NEEDS_CUSTOM_MOVE 0
#define TUNDRA_TYPE int
#define TUNDRA_TYPENAME int

// -----------------------------------------------------------------------------

// Create specialization for the given type
#include "tundra/internal/container_templates/LinkedList.h"

// Clean up
#undef TUNDRA_TYPE
#undef TUNDRA_TYPENAME
#undef TUNDRA_NEEDS_CUSTOM_COPY
#undef TUNDRA_NEEDS_CUSTOM_FREE
#undef TUNDRA_NEEDS_CUSTOM_MOVE

#endif // TUNDRA_LINKEDLISTint_H

