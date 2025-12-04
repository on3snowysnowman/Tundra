#!/bin/bash

# Script for generating DynamicArray code for a specified type.

if [[ $# -lt 1 ]]; then
    echo "Usage: $0 <TypeName>"
    exit 1
fi

TYPE=$1
HDR="../../include/tundra/containers/DynamicArray${TYPE}.h"
SRC="../../src/DynamicArray${TYPE}.c"

GUARD="TUNDRA_DYNARR${TYPE^^}_H"

# Common macro defs for this instantiation
DEFS=(
  "-DTYPE=${TYPE}"
  "-DNAME=TUNDRA_CONCAT(Tundra_DynamicArray, ${TYPE})"
  "-DFUNC_NAME(name)=TUNDRA_CONCAT3(Tundra_DynArr, ${TYPE}, _##name)"
  "-DINT_FUNC_NAME(name)=TUNDRA_CONCAT3(InTundra_DynArr, ${TYPE}, _##name)"
)

# Ensure library is up to date
cd ../../

scripts/build_linux.sh

cd tools/container_gen

# Generate header with type-specific include guard
{
    echo "#ifndef ${GUARD}"
    echo "#define ${GUARD}"
    echo
    # Run preprocessor as a pure text expander
    gcc -E -C -P "${DEFS[@]}" templates/DynamicArrayTemplate.h
    echo
    echo "#endif /* ${GUARD} */"
} > "${HDR}"

# Generate source file
gcc -E -C -P "${DEFS[@]}" templates/DynamicArrayTemplate.c > "${SRC}"

echo "Generated: ${HDR}, ${SRC}"

# Build library (after generation)

cd ../../

scripts/build_linux.sh
