#ifndef TUNDRA_DYNAMICARRAYint_H
#define TUNDRA_DYNAMICARRAYint_H

#include "tundra/internal/MacroHelper.h"

// Element handling flags for the template 
#define TUNDRA_NEEDS_CUSTOM_COPY 1
#define TUNDRA_NEEDS_CUSTOM_FREE 1
#define TUNDRA_NEEDS_CUSTOM_MOVE 1
#define TUNDRA_NEEDS_CUSTOM_INIT 1
#define TUNDRA_TYPE int

// COPY BEHAVIOR ---------------------------------------------------------------
/**
 * User-defined copy function for int.
 */
static inline void Tundra_DynArrint_custom_elem_copy(const int* src, int* dst)
{
    // User implements copy logic here.
    *dst = *src;
}

// Macro for per element copy call. Change the signature as needed, but macro 
// name must remain the same.
#define TUNDRA_COPY_FUNC_SIG(src_ptr, dst_ptr) \
    Tundra_DynArrint_custom_elem_copy((const int*)(src_ptr), (int*)(dst_ptr))

// FREE BEHAVIOR ---------------------------------------------------------------
/**
 * User-defined free function for int.
 */
static inline void Tundra_DynArrint_custom_elem_free(int* elem)
{
    // User implements free logic here.
}

// Macro for per element free call. Change the signature as needed, but macro 
// name must remain the same.
#define TUNDRA_FREE_FUNC_SIG(elem_ptr) \
    Tundra_DynArrint_custom_elem_free((int*)(elem_ptr))

// MOVE BEHAVIOR ---------------------------------------------------------------
/** 
 * User-defined move function for int.
 */
static inline void Tundra_DynArrint_custom_elem_move(int* src, int* dst)
{
    // User implements move logic here.
    *dst = *src;
}

// Macro for per element move call. Change the signature as needed, but macro 
// name must remain the same.
#define TUNDRA_MOVE_FUNC_SIG(src_ptr, dst_ptr) \
    Tundra_DynArrint_custom_elem_move((int*)(src_ptr), (int*)(dst_ptr))

// INIT BEHAVIOR ---------------------------------------------------------------
#define TUNDRA_PARAM(sig_format, type, name) , sig_format(type, name)

// Custom init function parameters macro. Change as needed, but macro name
// must remain the sam.e To add parameters, follow the examples below. The 
// `SIGNATURE_FORMAT` must be passed in as the first argument in the 
// TUNDRA_PARAM macro, then the sceond is the type, and the third is the name of
// the parameter. Do note that the element pointer to the element to initialize 
// is passed in as the first argument by default in the default 
// TUNDRA_INIT_FUNC_SIG below. Any parameters defined here should be 
// *in addition* to the element pointer. 
#define TUNDRA_INIT_PARAMS(SIGNATURE_FORMAT) \
    TUNDRA_PARAM(SIGNATURE_FORMAT, int, num) /** EXAMPLE PARAM \*/ 
    /** TUNDRA_PARAM(SIGNATURE_FORMAT, float, value) EXAMPLE PARAM */

// Macros for declaring and calling init function parameters. Do not change.
#define TUNDRA_DECL_PARAM(type, name) type name
#define TUNDRA_CALL_PARAM(type, name) name

/** 
 * User-defined init function for int.
 */
static inline void Tundra_DynArrint_custom_elem_init(int* elem 
	TUNDRA_INIT_PARAMS(TUNDRA_DECL_PARAM))
{
	// User implements init logic here.
    *elem = 0; // Example initialization

    num = *elem; // Example usage of additional parameter
}
// Macro for per element init call. Change the signature as needed, but macro 
// name must remain the same. `elem_ptr` is a pointer to the element to 
// initialize.
#define TUNDRA_INIT_FUNC_SIG(elem_ptr) Tundra_DynArrint_custom_elem_init(\
	elem_ptr TUNDRA_INIT_PARAMS(TUNDRA_CALL_PARAM))
// -----------------------------------------------------------------------------

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
#undef TUNDRA_COPY_FUNC_SIG
#undef TUNDRA_FREE_FUNC_SIG
#undef TUNDRA_MOVE_FUNC_SIG
#undef TUNDRA_INIT_FUNC_SIG

#endif // TUNDRA_DYNAMICARRAYint_H

