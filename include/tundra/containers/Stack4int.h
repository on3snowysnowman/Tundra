#ifndef TUNDRA_STACK4int_H
#define TUNDRA_STACK4int_H

// Type flags for the template
#define TUNDRA_TYPE int
#define TUNDRA_CAPACITY 4
#define TUNDRA_TYPENAME int

// Create specialization for the given type
#include "tundra/internal/container_templates/Stack.h"

// Clean up 
#undef TUNDRA_TYPE
#undef TUNDRA_CAPACITY
#undef TUNDRA_TYPENAME

#endif // TUNDRA_STACK4int_H
