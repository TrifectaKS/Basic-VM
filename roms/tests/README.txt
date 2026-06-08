Basic-VM Assembler Test Suite
=============================

This directory contains unit tests for the Basic-VM assembler.

Structure
----------
pass/           - Tests that should assemble successfully
  test_*.asm              - Assembly source files
  test_*_expected.bin     - Expected binary output (MD5 hash reference)

fail/           - Tests that should produce errors
  test_err_*.asm          - Assembly files that should fail

run_tests.sh     - Test runner script

Running Tests
-------------
From the project root:
  ./roms/tests/run_tests.sh

Or from this directory:
  cd roms/tests && ../..//output/asm

Test Categories
--------------
1. Single Instruction Tests
   - test_nop.asm through test_sys.asm
   - Verify each instruction encodes correctly

2. Label Tests
   - test_label_forward.asm - Branch to label defined later
   - test_label_backward.asm - Loop back to label
   - test_label_case.asm - Case insensitivity

3. Whitespace/Format Tests
   - test_comment_only.asm - Comments only (no output)
   - test_blank_lines.asm - Blank lines between instructions
   - test_mixed.asm - Mixed comments, labels, instructions
   - test_case_insensitive.asm - Instruction case insensitivity

4. Error Tests (fail/)
   - test_err_duplicate_label.asm - Duplicate label definition
   - test_err_undefined_label.asm - Reference to undefined label
   - test_err_unknown_inst.asm - Invalid instruction name
   - test_err_bad_reg.asm - Invalid register (r16)
   - test_err_bad_imm.asm - Immediate out of range

Hash Comparison
---------------
Pass tests compare output using MD5 hash:
  expected_hash = md5(test_*_expected.bin)
  output_hash = md5(test_output.bin)
  PASS if expected_hash == output_hash

Build Requirements
------------------
The test runner expects the assembler at:
  ../output/asm

Build the assembler first:
  ./bash/build_asm_macos.sh