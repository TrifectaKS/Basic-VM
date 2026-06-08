#!/bin/bash
# Build assembler without raylib dependency (debug + size optimized)

set -e

cd "$(dirname "$0")/.."
mkdir -p output

clang asm_main_stub.cpp src/asm/*.cpp \
    -Iinclude -Isrc \
    -o output/asm_debug_opt \
    -std=c++11 \
    -Os -flto -Wl,-x \
    -DDEBUG -DDEBUG_LOG

echo "Built: ./output/asm (debug + size optimized)"