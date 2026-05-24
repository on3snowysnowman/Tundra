#!/bin/bash

# printf "Array Test\n"
# ./bin/ArrayTest
# printf "\nDynamic Array Test\n"
# ./bin/DynArrTest
# printf "\nStack Test\n"
# ./bin/StackTest
# printf "\nDynamic Stack Test\n"
# ./bin/DynStkTest

for file in bin/*; do
    "$file"
done

