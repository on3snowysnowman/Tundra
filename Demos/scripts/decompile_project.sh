#!/bin/bash

./scripts/build_demos_linux.sh

objdump -d -M intel ./bin/demo > asm/decompiled_demo.asm
