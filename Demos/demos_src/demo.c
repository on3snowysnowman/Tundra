
/**
 * Generic Demo file for testing the Tundra Engine.
*/

#include <stdio.h>

#include <tundra_c/tundra_tools/DynamicArray.h>


int main()
{
    Tundra_DynamicArray array;

    Tundra_DynamicArray_init_array(&array, sizeof(int), 100);
    
    for(int i = 0; i < 15; ++i)
    {
        Tundra_DynamicArray_add_element(&array, &i);
    }

    for(int i = 0; i < array.num_elements; ++i)
    {
        int value = *(int*)(Tundra_DynamicArray_at(&array, i));

        printf("%d\n", value);
    }

    printf("Array Size: %ld\n", array.capacity);
    printf("Array Num Elements: %ld\n", array.num_elements);

    Tundra_DynamicArray_erase_element(&array, 3);

    for(int i = 0; i < array.num_elements; ++i)
    {
        int value = *(int*)(Tundra_DynamicArray_at(&array, i));

        printf("%d\n", value);
    }

    printf("Array Size: %ld\n", array.capacity);
    printf("Array Data Byte Size: %ld\n", 
        array.capacity * array.data_type_size);
    printf("Array Num Elements: %ld\n", array.num_elements);

    Tundra_DynamicArray_delete_array(&array);

    return 0;
}