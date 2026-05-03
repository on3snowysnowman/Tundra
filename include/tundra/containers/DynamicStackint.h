
#ifndef TUNDRA_DYNAMICSTACKint_H
#define TUNDRA_DYNAMICSTACKint_H

#include "tundra/internal/MacroHelper.h"

// Create dependant container
// -----------------------------------------------------------------------------
#ifndef TUNDRA_DYNAMICARRAYint_H
#define TUNDRA_TYPE int
#define TUNDRA_TYPENAME int
#include "tundra/internal/container_templates/DynamicArray.h"
#undef TUNDRA_TYPE
#undef TUNDRA_TYPENAME
#endif

// Type flags for the template 
#define TUNDRA_NEEDS_CUSTOM_COPY 1
#define TUNDRA_NEEDS_CUSTOM_FREE 1
#define TUNDRA_NEEDS_CUSTOM_MOVE 1
#define TUNDRA_NEEDS_CUSTOM_INIT 1
#define TUNDRA_TYPE int
#define TUNDRA_TYPENAME int

// COPY BEHAVIOR ---------------------------------------------------------------
// Macro for per element copy call. Change the signature as needed, but macro 
// name must remain the same. `src_ptr` points to the int to copy from, 
// `dst_ptr` points to the int to copy to. Assume `dst_ptr` does not 
// point to a valid object.
#define TUNDRA_COPY_CALL_SIG(dst_ptr, src_ptr) // User defines func call.

// FREE BEHAVIOR ---------------------------------------------------------------
// Macro for per element free call. Change the signature as needed, but macro 
// name must remain the same. `elem_ptr` points to the element to free.
#define TUNDRA_FREE_CALL_SIG(elem_ptr) // User defines func call.

// MOVE BEHAVIOR ---------------------------------------------------------------
// Macro for per element move call. Change the signature as needed, but macro 
// name must remain the same. `src_ptr` points to the int to move from,
// `dst_ptr` points to the int to move to. Assume `dst_ptr` does not 
// point to a valid object.
#define TUNDRA_MOVE_CALL_SIG(dst_ptr, src_ptr) // User defines func call.

// INIT BEHAVIOR ---------------------------------------------------------------
// Macro for per element default init call. Change the signature as needed, but 
// macro name must remain the same. `elem_ptr` points to the int to 
// default initialize. Assume `elem_ptr` does not point to a valid object.
#define TUNDRA_DEF_INIT_CALL_SIG(elem_ptr) // User defines func call

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
#undef TUNDRA_COPY_CALL_SIG
#undef TUNDRA_FREE_CALL_SIG
#undef TUNDRA_MOVE_CALL_SIG
#undef TUNDRA_DEF_INIT_CALL_SIG
#endif // TUNDRA_DYNAMICSTACKint_H
