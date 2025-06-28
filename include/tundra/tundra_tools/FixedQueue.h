/**
 * @file FixedQueue.h
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Generic fixed size container providing LIFO behavior for storing 
 *        elements.
 * @version 0.1
 * @date 06-27-25
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef TUNDRA_HGUARD_FIXEDQUEUE_H
#define TUNDRA_HGUARD_FIXEDQUEUE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>


#define TUNDRA_FIXEDQUEUE_TYPE_GENERATOR(type, name)                           \
typedef struct {                                                               \
    type* data;                                                                \
    uint64_t num_elements;                                                     \
    uint64_t capacity;                                                         \
} Tundra_FixedQueue##name;                                                     \
                                                                               \
static inline void Tundra_FxdQue##name##_init(Tundra_FixedQueue##name* queue,  \
    uint64_t cap)                                                              \
{                                                                              \
    queue->data = (type*)malloc(sizeof(type) * cap);                           \
    queue->num_elements = 0;                                                   \
    queue->capacity = cap;                                                     \
}                                                                              \
                                                                               \
static inline void Tundra_FxdQue##name##_deconstruct(                          \
    Tundra_FixedQueue##name* queue)                                            \
{                                                                              \
    free(queue->data);                                                         \
    queue->data = NULL;                                                        \
    queue->num_elements = 0;                                                   \
    queue->capacity = 0;                                                       \
}                                                                              \
                                                                               \
static inline void Tundra_FxdQue##name##_push(Tundra_FixedQueue##name* queue,  \
    type element)                                                              \
{                                                                              \
    if (queue->num_elements == queue->capacity) return;                        \
    queue->data[queue->num_elements++] = element;                              \
}                                                                              \
                                                                               \
static inline void Tundra_FxdQue##name##_pop(Tundra_FixedQueue##name* queue)   \
{                                                                              \               
    if (queue->num_elements > 0) queue->num_elements--;                        \
}                                                                              \
                                                                               \
static inline void Tundra_FxdQue##name##_clear(Tundra_FixedQueue##name* queue) \
{                                                                              \
    queue->num_elements = 0;                                                   \
}                                                                              \
                                                                               \
static inline bool Tundra_FxdQue##name##_is_empty(                             \
    const Tundra_FixedQueue##name* queue)                                      \
{                                                                              \
    return queue->num_elements == 0;                                           \
}                                                                              \
                                                                               \
static inline bool Tundra_FxdQue##name##_is_full(                              \
    const Tundra_FixedQueue##name* queue)                                      \
{                                                                              \                                     
    return queue->num_elements == queue->capacity;                             \
}                                                                              \
                                                                               \
static inline type* Tundra_FxdQue##name##_front(                               \
    const Tundra_FixedQueue##name* queue)                                      \
{                                                                              \              
    if (queue->num_elements == 0) return NULL;                                 \
    return &queue->data[queue->num_elements - 1];                              \
}
// TUNDRA_DEFINED_FIXEDQUEUE_TYPE


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

//     // Number of elements that are currently in the queue.
//     uint64_t num_elements;

//     // Max number of elements the Queue can stored.
//     uint64_t capacity;

// } Tundra_FixedQueue;


// /**
//  * @brief Initializes the Queue, allocating memory for 'capacity' num elements 
//  * of size 'data_type_size' and setting internal components.
//  * 
//  * @param queue Queue to initialize.
//  * @param data_type_size Size of the data type stored.
//  * @param capacity Capacity of the Queue in number of elements.
//  */
// void Tundra_FxdQue_init(Tundra_FixedQueue *queue, uint32_t data_type_size,
//     uint64_t capacity); 

// /**
//  * @brief Handles deletion of heap allocated memory for this Queue.
//  * 
//  * The Queue can be safely discarded after this method is called.
//  * 
//  * @param queue 
//  */
// void Tundra_FxdQue_deconstruct(Tundra_FixedQueue *queue);

// /**
//  * @brief Pushes an element(lvalue) onto the Queue.
//  * 
//  * If you want to push a rvalue onto the Queue, use the macro 
//  * `TUNDRA_FXDQUE_PUSH_RVAL`. 
//  * 
//  * THe bytes of the pointed to element are copied into the Queue.
//  * 
//  * Performs a check if the Queue is full. If it is, the element is not added.
//  * 
//  * @param queue Queue to modify.
//  * @param element Element to push.
//  */
// void Tundra_FxdQue_push(Tundra_FixedQueue *queue, void *element);

// /**
//  * @brief Removes the top element from the Queue.
//  * 
//  * This function does not return the removed element. To access the element 
//  * before removal, use the `front` function to obtain a pointer to it, copy the 
//  * data if needed, and then call this function to remove it.
//  * 
//  * @param queue Queue to modify.
//  */
// void Tundra_FxdQue_pop(Tundra_FixedQueue *queue);

// /**
//  * @brief "Clears" the queue of added elements.
//  * 
//  * Internally, no memory is zeroed out or overridden. The variable for tracking
//  * the number of elements is simply set to 0 so that when new elements are 
//  * pushed, they will overwrite the old data starting at the beginning of the 
//  * Queue.
//  * 
//  * @param queue Queue to clear.
//  */
// void Tundra_FxdQue_clear(Tundra_FixedQueue *queue);

// /**
//  * @brief Returns true if the Queue is empty.
//  * 
//  * @param queue Queue to check.
//  */
// bool Tundra_FxdQue_is_empty(Tundra_FixedQueue *queue);

// /**
//  * @brief Returns true if the Queue is full.
//  * 
//  * @param queue Queue to check.
//  */
// bool Tundra_FxdQue_is_full(Tundra_FixedQueue *queue);

// /**
//  * @brief Retrieves a pointer to the element at the front of the Queue.
//  * 
//  * If the Queue is empty, NULL is returned.
//  * 
//  * @param queue Queue to fetch from.
//  */
// void* Tundra_FxdQue_front(Tundra_FixedQueue *queue);



#endif // TUNDRA_HGUARD_FIXEDQUEUE_H
