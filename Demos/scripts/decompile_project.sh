#!/bin/bash

./scripts/build_demos_linux.sh

if [ ! -d "asm" ]; then
mkdir asm
fi

objdump -d -M intel ./bin/demo > asm/decompiled_demo.asm
