#ifndef TUNDRA_DYNAMICARRAYint_H
#define TUNDRA_DYNAMICARRAYint_H

// Element handling flags for the template 
#define TUNDRA_NEEDS_NEEDS_CUSTOM_COPY 1
#define TUNDRA_NEEDS_CUSTOM_FREE 1
#define TUNDRA_NEEDS_CUSTOM_MOVE 1
#define TUNDRA_TYPE int

/**
 * User-defined copy function for int. Default generated, change as needed.
 */
static inline void Tundra_DynArrint_custom_elem_copy(const int* src, int* dst)
{
    // User implements copy logic here.
}

// Macro for per element copy call. Change the signature as needed, but macro 
// name must remain the same.
#define TUNDRA_COPY_FUNC_SIG(dst_ptr, src_ptr) \
  Tundra_DynArrint_custom_elem_copy((const int*)(src_ptr), (int*)(dst_ptr))

/**
 * User-defined free function for int. Default generated, change as needed.
 */
static inline void Tundra_DynArrint_custom_elem_free(int* array)
{
    // User implements free logic here.
}

// Macro for per element free call. Change the signature as needed, but macro 
// name must remain the same.
#define TUNDRA_FREE_FUNC_SIG(array_ptr) \
    Tundra_DynArrint_custom_elem_free((int*)(array_ptr))

/** 
 * User-defined move function for int. Default generated, change as needed.
 */
static inline void Tundra_DynArrint_custom_elem_move(int* dst, int* src)
{
    // User implements move logic here.
}

// Macro for per element move call. Change the signature as needed, but macro 
// name must remain the same.
#define TUNDRA_MOVE_FUNC_SIG(dst_ptr, src_ptr) \
    Tundra_DynArrint_custom_elem_move((int*)(dst_ptr), (int*)(src_ptr))

// Create specialization for the given type
#include "tundra/internal/container_templates/DynamicArray.h"

// Clean up
#undef TUNDRA_TYPE
#undef TUNDRA_NEEDS_NEEDS_CUSTOM_COPY
#undef TUNDRA_NEEDS_CUSTOM_FREE
#undef TUNDRA_NEEDS_CUSTOM_MOVE

#ifdef TUNDRA_COPY_FUNC_SIG
#undef TUNDRA_COPY_FUNC_SIG
#endif

#ifdef TUNDRA_FREE_FUNC_SIG
#undef TUNDRA_FREE_FUNC_SIG
#endif

#ifdef TUNDRA_MOVE_FUNC_SIG
#undef TUNDRA_MOVE_FUNC_SIG
#endif

#endif // TUNDRA_DYNAMICARRAYint_H

