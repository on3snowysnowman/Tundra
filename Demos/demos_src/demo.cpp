
#include "tundra/utils/memory/MemoryAlloc.hpp"

#include <time.h>
#include <string.h>

int main()
{
    srand(time(NULL));

    void *src, *dst;

    for(int i = 1; i < 10000; ++i)
    {
        uint64_t num_bytes = i * 10;

        src = malloc(num_bytes);
        dst = malloc(num_bytes);

        Tundra::copy_mem(src, dst, num_bytes);
        // memcpy(dst, src, num_bytes);

        free(src);
        free(dst);
    }
}