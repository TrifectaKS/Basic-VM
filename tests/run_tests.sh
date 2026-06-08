#!/bin/bash
# Test runner for Basic-VM Assembler
# Compares output against expected using MD5 hash

TESTS_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$TESTS_DIR"

ASM="../output/asm"
PASS_DIR="pass"
FAIL_DIR="fail"

PASS_COUNT=0
FAIL_COUNT=0
TOTAL_PASS=0
TOTAL_FAIL=0

echo "========================================"
echo "Basic-VM Assembler Test Suite"
echo "========================================"
echo ""

# Ensure assembler exists
if [ ! -f "$ASM" ]; then
    echo "ERROR: Assembler not found at $ASM"
    echo "Build with: ./bash/build_asm_macos.sh"
    exit 1
fi

# Run pass tests
echo "--- PASS TESTS ---"
echo ""

for asm_file in $PASS_DIR/*.asm; do
    if [ ! -f "$asm_file" ]; then
        continue
    fi

    test_name=$(basename "$asm_file" .asm)
    expected_file="${asm_file%.asm}_expected.bin"
    output_file="/tmp/test_output.bin"

    # Skip if no expected file
    if [ ! -f "$expected_file" ]; then
        echo "[SKIP] $test_name (no expected file)"
        continue
    fi

    # Assemble
    $ASM "$asm_file" "$output_file" > /dev/null 2>&1

    if [ $? -ne 0 ]; then
        echo "[FAIL] $test_name (assembly failed)"
        FAIL_COUNT=$((FAIL_COUNT + 1))
        continue
    fi

    # Compare hashes
    expected_hash=$(md5 -q "$expected_file" 2>/dev/null)
    output_hash=$(md5 -q "$output_file" 2>/dev/null)

    if [ "$expected_hash" = "$output_hash" ]; then
        echo "[PASS] $test_name"
        PASS_COUNT=$((PASS_COUNT + 1))
    else
        echo "[FAIL] $test_name"
        echo "  Expected: $expected_hash"
        echo "  Got:      $output_hash"
        FAIL_COUNT=$((FAIL_COUNT + 1))
    fi
done

TOTAL_PASS=$((TOTAL_PASS + PASS_COUNT))
TOTAL_FAIL=$((TOTAL_FAIL + FAIL_COUNT))
PASS_COUNT=0
FAIL_COUNT=0

echo ""
echo "--- FAIL TESTS (should produce errors) ---"
echo ""

for asm_file in $FAIL_DIR/*.asm; do
    if [ ! -f "$asm_file" ]; then
        continue
    fi

    test_name=$(basename "$asm_file" .asm)
    output_file="/tmp/test_output.bin"
    log_file="${output_file}.log"

    # Attempt to assemble
    $ASM "$asm_file" "$output_file" > /dev/null 2>&1
    exit_code=$?
    [ $exit_code -eq 0 ] && exit_code=1 || exit_code=$exit_code

    # Check for error (exit code != 0 OR log file exists)
    if [ $exit_code -ne 0 ] || [ -f "$log_file" ]; then
        echo "[PASS] $test_name (correctly produced error)"
        PASS_COUNT=$((PASS_COUNT + 1))
    else
        echo "[FAIL] $test_name (should have failed)"
        FAIL_COUNT=$((FAIL_COUNT + 1))
    fi

    # Cleanup
    rm -f "$output_file" "$log_file"
done

TOTAL_PASS=$((TOTAL_PASS + PASS_COUNT))
TOTAL_FAIL=$((TOTAL_FAIL + FAIL_COUNT))

echo ""
echo "========================================"
echo "RESULTS"
echo "========================================"
echo "Pass: $TOTAL_PASS"
echo "Fail: $TOTAL_FAIL"
echo ""

if [ $TOTAL_FAIL -eq 0 ]; then
    echo "All tests passed!"
    exit 0
else
    echo "Some tests failed."
    exit 1
fi
