
#ifndef TUNDRA_ARRAYCCharPtr128_H
#define TUNDRA_DYNAMICARRAYCCharPtr128_H

#include "tundra/internal/MacroHelper.h"

#define TUNDRA_TYPE const char *
#define TUNDRA_TYPENAME CCharPtr
#define TUNDRA_CAPACITY 128

// Create specialization for the given type
#include "tundra/internal/container_templates/Array.h"

#endif // TUNDRA_ARRCCharPtr128_H
