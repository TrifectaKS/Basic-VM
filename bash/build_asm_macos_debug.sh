#!/bin/bash
# Build assembler without raylib dependency (debug enabled)

set -e

cd "$(dirname "$0")/.."
mkdir -p output

clang asm_main_stub.cpp src/asm/*.cpp \
    -Iinclude -Isrc \
    -o output/asm_debug \
    -std=c++11 \
    -DDEBUG

echo "Built: ./output/asm (debug)"