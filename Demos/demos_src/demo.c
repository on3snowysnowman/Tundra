
/**
 * Generic Demo file for testing Tundra Components.
*/

#include <tundra/tundra_tools/FixedQueue.h>

int main()
{
    Tundra_FixedQueue queue;

    Tundra_FxdQue_init(&queue, sizeof(int), 10);

    TUNDRA_FXDQUE_PUSH_RVAL(&queue, int, 4);

    Tundra_FxdQue_deconstruct(&queue);
    
    return 0;
}
