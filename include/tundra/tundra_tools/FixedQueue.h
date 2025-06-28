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


/**
 * @brief Generic fixed size container providing LIFO behavior for storing
 * elements.
 * 
 * Users should NOT modify internal variables, they are read-only.
 */
typedef struct
{
    // Pointer to the data on the heap.
    void* data;

    // Size of the data type stored.
    uint32_t data_type_size;

    // Number of elements that are currently in the queue.
    uint64_t num_elements;

    // Max number of elements the Queue can stored.
    uint64_t capacity;

} Tundra_FixedQueue;


/**
 * @brief Initializes the Queue, allocating memory for 'capacity' num elements 
 * of size 'data_type_size' and setting internal components.
 * 
 * @param queue Queue to initialize.
 * @param data_type_size Size of the data type stored.
 * @param capacity Capacity of the Queue in number of elements.
 */
void Tundra_FxdQue_init(Tundra_FixedQueue *queue, uint32_t data_type_size,
    uint64_t capacity); 

/**
 * @brief Pushes an element onto the queue.
 * 
 * Performs a check if the queue is full, and will throw an error if it is.
 * 
 * @param queue Queue to modify.
 * @param element Element to push.
 */
void Tundra_FxdQue_push(Tundra_FixedQueue *queue, void* element);

void Tundra_FxdQue_pop(Tundra_FixedQueue *queue);

void* Tundra_FxdQue_front(Tundra_FixedQueue *queue);


#endif // TUNDRA_HGUARD_FIXEDQUEUE_H