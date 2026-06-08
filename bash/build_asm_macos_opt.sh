#!/bin/bash
# Build assembler without raylib dependency (size optimized)

set -e

cd "$(dirname "$0")/.."
mkdir -p output

clang asm_main_stub.cpp src/asm/*.cpp \
    -Iinclude -Isrc \
    -o output/asm \
    -std=c++11 \
    -Os -s -flto

echo "Built: ./output/asm (size optimized)"