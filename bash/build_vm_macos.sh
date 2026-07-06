#!/bin/bash
# Build VM without raylib dependency

set -e

cd "$(dirname "$0")/.."
mkdir -p output

clang vm_main_stub.cpp src/vm/*.cpp \
    -Iinclude -Isrc \
    -o output/vm \
    -std=c++11

echo "Built: ./output/vm"
