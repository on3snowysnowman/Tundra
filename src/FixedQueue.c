/**
 * @file FixedQueue.c
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Generic fixed size container providing LIFO behavior for storing 
 *        elements.
 * @version 0.1
 * @date 06-27-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "tundra/tundra_tools/FixedQueue.h"

#include <stdlib.h>
#include <string.h>


void Tundra_FxdQue_init(Tundra_FixedQueue *queue, uint32_t data_type_size, 
    uint64_t capacity)
{
    // Allocate memory for the queue.
    queue->data = malloc(capacity * data_type_size);

    queue->data_type_size = data_type_size;
    queue->num_elements = 0;
    queue->capacity = capacity;
}

void Tundra_FxdQue_deconstruct(Tundra_FixedQueue *queue)
{
    free(queue->data);
    queue->data = NULL;
}

void Tundra_FxdQue_push(Tundra_FixedQueue *queue, void *element)
{
    // If the Queue is full.
    if(queue->num_elements == queue->capacity) return;

    // Copy bytes from the element to the queue.
    memcpy((char*)(queue->data) + (queue->num_elements * queue->data_type_size), 
        element, queue->data_type_size);

    ++queue->num_elements;
}

void Tundra_FxdQue_pop(Tundra_FixedQueue *queue)
{
    queue->num_elements -= queue->num_elements != 0;
    --queue->num_elements;
}

void Tundra_FxdQue_clear(Tundra_FixedQueue *queue)
{
    queue->num_elements = 0;
}

bool Tundra_FxdQue_is_empty(Tundra_FixedQueue *queue)
{
    return queue->num_elements == 0;
}

bool Tundra_FxdQue_is_full(Tundra_FixedQueue *queue)
{
    return queue->num_elements == queue->capacity;
}

void* Tundra_FxdQue_front(Tundra_FixedQueue *queue)
{
    if(queue->num_elements == 0) return NULL;

    return (char*)(queue->data) + 
        ((queue->num_elements - 1) * queue->data_type_size);
}


