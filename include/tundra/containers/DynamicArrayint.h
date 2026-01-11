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

static inline void copy_int(const int *src, int *dst)
{
    *dst = *src;
}

// Macro for per element copy call. Change the signature as needed, but macro 
// name must remain the same. `src_ptr` points to the int to copy from, 
// `dst_ptr` points to the int to copy to. Assume `dst_ptr` points to
// junk.
#define TUNDRA_COPY_CALL_SIG(src_ptr, dst_ptr) copy_int(src_ptr, dst_ptr)

// FREE BEHAVIOR ---------------------------------------------------------------

static inline void free_int(int *elem)
{
    // noop
}

// Macro for per element free call. Change the signature as needed, but macro 
// name must remain the same. `elem_ptr` points to the element to free.
#define TUNDRA_FREE_CALL_SIG(elem_ptr) free_int(elem_ptr)

// MOVE BEHAVIOR ---------------------------------------------------------------

static inline void move_int(int *src, int *dst)
{
    *dst = *src;

    *src = 0;
}

// Macro for per element move call. Change the signature as needed, but macro 
// name must remain the same. `src_ptr` points to the int to move from,
// `dst_ptr` points to the int to move to. Assume `dst_ptr` points to
// junk.
#define TUNDRA_MOVE_CALL_SIG(src_ptr, dst_ptr) move_int(src_ptr, dst_ptr)

// INIT BEHAVIOR ---------------------------------------------------------------

// Macro for defining an init parameter for the init parameter list. User should
// not modify.
#define TUNDRA_PARAM(type, name) , type name

// Parameter list for the init call of the given type. Change the signature as 
// needed, but macro name must remain the same. Use the macro TUNDRA_PARAM to 
// define the list like so: 
// #define TUNDRA_INIT_PARAM_LIST \
//    TUNDRA_PARAM(int, num) \
//    TUNDRA_PARAM(float, value)
//
// This example defines two parameters for the init call: int num and float 
// value.
//
// The parameter list is allowed to be empty if there are no parameters, simply
// leave the macro signature as empty.
//
#define TUNDRA_INIT_PARAM_LIST \
    TUNDRA_PARAM(int, num) \ 
    TUNDRA_PARAM(float, value)

static inline void def_init_int(int *elem)
{
    *elem = 0;
}

// Macro for per element default init call. Change the signature as needed, but
// macro name must remain the same. `elem_ptr` points to the int to 
// default initialize. Assume the pointed to data is not yet a valid object.
#define TUNDRA_DEF_INIT_CALL_SIG(elem_ptr) def_init_int(elem)

static inline void init_int(int *elem, int num, float value)
{
    *elem = num;

    value = 0;

}

// Macro for per element initialization call with parameters. Change the 
// signature as needed, but macro name must remain the same. `elem_ptr` 
// pointers to the element to initialize. Assume the pointed to data is not yet
// a valid object. 
// 
// In the example below, the function parameters are laid out with the elem_ptr
// first, then with the parameter list following it. This does not necessarily 
// have to be the layout of the call, just ensure that any parameters inside 
// the function signature are either `elem_ptr` or any parameter names 
// defined in the TUNDRA_INIT_PARAM_LIST macro.
#define TUNDRA_INIT_CALL_SIG(elem_ptr) init_int(elem_ptr, num, value)
    // some_init_func(elem_ptr, num, value)

// -----------------------------------------------------------------------------

// Create specialization for the given type
#include "tundra/internal/container_templates/DynamicArray.h"

// Clean up
#undef TUNDRA_TYPE
#undef TUNDRA_TYPENAME
#undef TUNDRA_NEEDS_CUSTOM_COPY
#undef TUNDRA_NEEDS_CUSTOM_FREE
#undef TUNDRA_NEEDS_CUSTOM_MOVE
#undef TUNDRA_COPY_CALL_SIG
#undef TUNDRA_FREE_CALL_SIG
#undef TUNDRA_MOVE_CALL_SIG

#endif // TUNDRA_DYNAMICARRAYint_H

