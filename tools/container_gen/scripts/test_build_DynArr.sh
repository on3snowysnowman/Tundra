# !/bin/bash

# Script for building an int type Dynamic Array container to verify compilation.

# Make sure lib is up to date
cd ../../

scripts/build_linux.sh

cd tools/container_gen

gcc ./templates/DynamicArrayTemplate.c -c -Wall -Wextra -o bin/DynArr.o \
-DTYPE=float