#!/bin/bash
# Build assembler without raylib dependency

set -e

cd "$(dirname "$0")/.."
mkdir -p output

clang asm_main_stub.cpp src/asm/*.cpp \
    -Iinclude -Isrc \
    -o output/asm \
    -std=c++11

echo "Built: ./output/asm"