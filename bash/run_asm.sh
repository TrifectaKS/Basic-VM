#!/bin/bash
# Run the assembler with CLI arguments
# Usage: ./run_asm.sh <input.asm> <output.rom>

set -e

cd "$(dirname "$0")/.."

if [ $# -ne 2 ]; then
    echo "Usage: $0 <input.asm> <output.rom>"
    exit 1
fi

./output/asm_debug_opt "$1" "$2"