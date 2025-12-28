#ifndef TUNDRA_DYNAMICSTACKint_H
#define TUNDRA_DYNAMICSTACKint_H

#include "tundra/internal/MacroHelper.h"

// Element handling flags for the template 
#define TUNDRA_NEEDS_CUSTOM_COPY 1
#define TUNDRA_NEEDS_CUSTOM_FREE 1
#define TUNDRA_NEEDS_CUSTOM_MOVE 1
#define TUNDRA_TYPE int
#define TUNDRA_TYPENAME int

// COPY BEHAVIOR ---------------------------------------------------------------

void copy_int(const int *src_ptr, int *dst_ptr)
{
    *dst_ptr = *src_ptr;
}

// Macro for per element copy call. Change the signature as needed, but macro 
// name must remain the same.
#define TUNDRA_COPY_CALL_SIG(src_ptr, dst_ptr) copy_int(src_ptr, dst_ptr);

// FREE BEHAVIOR ---------------------------------------------------------------

// Macro for per element free call. Change the signature as needed, but macro 
// name must remain the same.
#define TUNDRA_FREE_CALL_SIG(elem_ptr) // User defines func call.

// MOVE BEHAVIOR ---------------------------------------------------------------

void move_int(int *src_ptr, int *dst_ptr)
{
    *dst_ptr = *src_ptr;

    *src_ptr = 0;
}

// Macro for per element move call. Change the signature as needed, but macro 
// name must remain the same.
#define TUNDRA_MOVE_CALL_SIG(src_ptr, dst_ptr) move_int(src_ptr, dst_ptr);

// -----------------------------------------------------------------------------

// Create specialization for the DYNAMICSTACK which is required by the DynStk
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
#undef TUNDRA_COPY_FUNC_SIG
#undef TUNDRA_FREE_FUNC_SIG
#undef TUNDRA_MOVE_FUNC_SIG

#endif // TUNDRA_DYNAMICSTACKint_H

