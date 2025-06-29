/**
 * @file FixedStack.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Generic fixed size container providing LIFO behavior for storing 
 *        elements.
 * @version 0.1
 * @date 06-27-25
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef TUNDRA_HGUARD_FIXEDSTACK_H
#define TUNDRA_HGUARD_FIXEDSTACK_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>


#define TUNDRA_FIXEDSTACK_TYPE_GENERATOR(type, name)                           \
typedef struct {                                                               \
    type* data;                                                                \
    uint64_t num_elements;                                                     \
    uint64_t capacity;                                                         \
} Tundra_FixedStack##name;                                                     \
                                                                               \
static inline void Tundra_FxdStk##name##_init(Tundra_FixedStack##name* stack,  \
    uint64_t cap)                                                              \
{                                                                              \
    stack->data = (type*)malloc(sizeof(type) * cap);                           \
    stack->num_elements = 0;                                                   \
    stack->capacity = cap;                                                     \
}                                                                              \
                                                                               \
static inline void Tundra_FxdStk##name##_deconstruct(                          \
    Tundra_FixedStack##name* stack)                                            \
{                                                                              \
    free(stack->data);                                                         \
    stack->data = NULL;                                                        \
    stack->num_elements = 0;                                                   \
    stack->capacity = 0;                                                       \
}                                                                              \
                                                                               \
static inline void Tundra_FxdStk##name##_push(Tundra_FixedStack##name* stack,  \
    type element)                                                              \
{                                                                              \
    if (stack->num_elements == stack->capacity) return;                        \
    stack->data[stack->num_elements++] = element;                              \
}                                                                              \
                                                                               \
static inline void Tundra_FxdStk##name##_pop(Tundra_FixedStack##name* stack)   \
{                                                                              \               
    if (stack->num_elements > 0) stack->num_elements--;                        \
}                                                                              \
                                                                               \
static inline void Tundra_FxdStk##name##_clear(Tundra_FixedStack##name* stack) \
{                                                                              \
    stack->num_elements = 0;                                                   \
}                                                                              \
                                                                               \
static inline bool Tundra_FxdStk##name##_is_empty(                             \
    const Tundra_FixedStack##name* stack)                                      \
{                                                                              \
    return stack->num_elements == 0;                                           \
}                                                                              \
                                                                               \
static inline bool Tundra_FxdStk##name##_is_full(                              \
    const Tundra_FixedStack##name* stack)                                      \
{                                                                              \                                     
    return stack->num_elements == stack->capacity;                             \
}                                                                              \
                                                                               \
static inline type* Tundra_FxdStk##name##_front(                               \
    const Tundra_FixedStack##name* stack)                                      \
{                                                                              \              
    if (stack->num_elements == 0) return NULL;                                 \
    return &stack->data[stack->num_elements - 1];                              \
}
// TUNDRA_DEFINED_FIXEDSTACK_TYPE



// /**
//  * @brief Generic fixed size container providing LIFO behavior for storing
//  * elements.
//  * 
//  * Users should NOT modify internal variables, they are read-only.
//  */
// typedef struct
// {
//     // Pointer to the data on the heap.
//     void* data;

//     // Size of the data type stored.
//     uint32_t data_type_size;

//     // Number of elements that are currently in the stack.
//     uint64_t num_elements;

//     // Max number of elements the Stack can store.
//     uint64_t capacity;

// } Tundra_FixedStack;


// /**
//  * @brief Initializes the Stack, allocating memory for 'capacity' num elements 
//  * of size 'data_type_size' and setting internal components.
//  * 
//  * @param stack Stack to initialize.
//  * @param data_type_size Size of the data type stored.
//  * @param capacity Capacity of the Stack in number of elements.
//  */
// void Tundra_FxdStk_init(Tundra_FixedStack *stack, uint32_t data_type_size,
//     uint64_t capacity); 

// /**
//  * @brief Handles deletion of heap allocated memory for this Stack.
//  * 
//  * The Stack can be safely discarded after this method is called.
//  * 
//  * @param stack 
//  */
// void Tundra_FxdStk_deconstruct(Tundra_FixedStack *stack);

// /**
//  * @brief Pushes an element(lvalue) onto the Stack.
//  * 
//  * If you want to push a rvalue onto the Stack, use the macro 
//  * `TUNDRA_FXDQUE_PUSH_RVAL`. 
//  * 
//  * THe bytes of the pointed to element are copied into the Stack.
//  * 
//  * Performs a check if the Stack is full. If it is, the element is not added.
//  * 
//  * @param stack Stack to modify.
//  * @param element Element to push.
//  */
// void Tundra_FxdStk_push(Tundra_FixedStack *stack, void *element);

// /**
//  * @brief Removes the top element from the Stack.
//  * 
//  * This function does not return the removed element. To access the element 
//  * before removal, use the `front` function to obtain a pointer to it, copy the 
//  * data if needed, and then call this function to remove it.
//  * 
//  * @param stack Stack to modify.
//  */
// void Tundra_FxdStk_pop(Tundra_FixedStack *stack);

// /**
//  * @brief "Clears" the stack of added elements.
//  * 
//  * Internally, no memory is zeroed out or overridden. The variable for tracking
//  * the number of elements is simply set to 0 so that when new elements are 
//  * pushed, they will overwrite the old data starting at the beginning of the 
//  * Stack.
//  * 
//  * @param stack Stack to clear.
//  */
// void Tundra_FxdStk_clear(Tundra_FixedStack *stack);

// /**
//  * @brief Returns true if the Stack is empty.
//  * 
//  * @param stack Stack to check.
//  */
// bool Tundra_FxdStk_is_empty(Tundra_FixedStack *stack);

// /**
//  * @brief Returns true if the Stack is full.
//  * 
//  * @param stack Stack to check.
//  */
// bool Tundra_FxdStk_is_full(Tundra_FixedStack *stack);

// /**
//  * @brief Retrieves a pointer to the element at the front of the Stack.
//  * 
//  * If the Stack is empty, NULL is returned.
//  * 
//  * @param stack Stack to fetch from.
//  */
// void* Tundra_FxdStk_front(Tundra_FixedStack *stack);

#endif // TUNDRA_HGUARD_FIXEDSTACK_H
