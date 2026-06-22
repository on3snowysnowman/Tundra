
#ifndef TUNDRA_DYNAMICARRAYint_H
#define TUNDRA_DYNAMICARRAYint_H

#include "tundra/internal/MacroHelper.h"

// Type flags for the template 
#define TUNDRA_NEEDS_CUSTOM_COPY 1
#define TUNDRA_NEEDS_CUSTOM_FREE 1
#define TUNDRA_NEEDS_CUSTOM_MOVE 1
#define TUNDRA_NEEDS_CUSTOM_INIT 1
#define TUNDRA_TYPE int
#define TUNDRA_TYPENAME int

// COPY BEHAVIOR ---------------------------------------------------------------

// Macro for per element copy call from a source object to an uninitialized 
// object. Change the signature as needed, but macro name must remain the same.
#define TUNDRA_COPY_INIT_CALL_SIG(dst_ptr, src_ptr) // User defines func call.

// Macro for per element copy call from a source object to an initialized 
// object. Change the signature as needed, but macro name must remain the same.
#define TUNDRA_COPY_ASSIGN_CALL_SIG(dst_ptr, src_ptr) // User defines func call.

// FREE BEHAVIOR ---------------------------------------------------------------
// Macro for per element free call. Change the signature as needed, but macro 
// name must remain the same. `elem_ptr` points to the element to free.
#define TUNDRA_FREE_CALL_SIG(elem_ptr) // User defines func call.

// MOVE BEHAVIOR ---------------------------------------------------------------

// Macro for per element move call from a source object to an uninitialized 
// object. Change the signature as needed, but macro name must remain the same.
#define TUNDRA_MOVE_INIT_CALL_SIG(dst_ptr, src_ptr) // User defines func call.

// Macro for per element move call from a source object to an initialized 
// object. Change the signature as needed, but macro name must remain the same.
#define TUNDRA_MOVE_ASSIGN_CALL_SIG(dst_ptr, src_ptr) // User defines func call.

// INIT BEHAVIOR ---------------------------------------------------------------

// Macro for per element default init call on an uninitialized object.
// Change the signature as needed, but macro name must remain the same.
#define TUNDRA_DEF_INIT_CALL_SIG(elem_ptr) // User defines func call

// -----------------------------------------------------------------------------

// Create specialization for the given type
#include "tundra/internal/container_templates/DynamicArray.h"

// Clean up
#undef TUNDRA_TYPE
#undef TUNDRA_TYPENAME
#undef TUNDRA_NEEDS_CUSTOM_COPY
#undef TUNDRA_NEEDS_CUSTOM_FREE
#undef TUNDRA_NEEDS_CUSTOM_MOVE
#undef TUNDRA_NEEDS_CUSTOM_INIT
#undef TUNDRA_COPY_INIT_CALL_SIG
#undef TUNDRA_COPY_ASSIGN_CALL_SIG
#undef TUNDRA_FREE_CALL_SIG
#undef TUNDRA_MOVE_INIT_CALL_SIG
#undef TUNDRA_MOVE_ASSIGN_CALL_SIG
#undef TUNDRA_DEF_INIT_CALL_SIG
#endif // TUNDRA_DYNAMICARRAYint_H
