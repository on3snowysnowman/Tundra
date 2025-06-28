/**
 * @file FixedQueue.c
 * @author Joel Height (On3SnowySnowman@gmail.com)
 * @brief Generic fixed size container providing FIFO behavior for storing 
 *        elements.
 * @version 0.1
 * @date 06-27-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "tundra/tundra_tools/FixedQueue.h"

#include <stdlib.h>

// Private Methods -------------------------------------------------------------


// Public Methods --------------------------------------------------------------

void Tundra_FxdQue_init(Tundra_FixedQueue *queue, uint32_t data_type_size, 
    uint64_t capacity)
{
    // Allocate memory for the queue.
    queue->data = malloc(capacity * data_type_size);

    queue->data_type_size = data_type_size;
    queue->num_elements = 0;
    queue->capacity = capacity;
}


