# Basic-VM Assembler - Agent Documentation

## Project Structure
```
Basic-VM/
├── src/asm/           # Assembler source (.cpp files)
├── include/           # Header files (.h files)
├── bash/              # Build scripts
│   ├── build_asm_macos.sh       # Basic build
│   ├── build_asm_macos_opt.sh   # Size optimized
│   ├── build_asm_macos_debug.sh # Debug enabled
│   ├── build_asm_macos_debug_opt.sh # Debug + optimized
│   ├── run_asm.sh               # Run assembler
│   └── README.txt               # Build documentation
├── output/            # Compiled binary output
├── roms/              # Input assembly files
└── docs/              # ISA and PLAN documentation
```

## Build System
- **4 build variants** controlled by `-D` flags at compile time
- **DEBUG flag**: Enables `printf` debug output during assembly
- **DEBUG_LOG flag**: Enables error logging to `<output>.log`

## Build Commands
```bash
./bash/build_asm_macos.sh          # Basic
./bash/build_asm_macos_opt.sh      # Optimized
./bash/build_asm_macos_debug.sh     # Debug
./bash/build_asm_macos_debug_opt.sh # Debug + Optimized
```

## Running the Assembler
```bash
./bash/run_asm.sh <input.asm> <output.rom>
```

## Assembler Features
- **Two-pass assembly**: Labels and variables collected in pass 1, resolved in pass 2
- **Label support**: Global labels for branches/jumps (32 chars max, case insensitive)
- **Variable support**: Named constants for immediate values (32 chars max, case insensitive, 12-bit bounds)
- **Error logging**: When DEBUG_LOG is defined, errors written to `<output>.log`
- **Output format**: 32-bit instructions, little-endian

### ASM Variables
Variables are named constants that can be used anywhere an immediate value is expected:
```asm
BANK_ADDR = 0x100
CONFIG_VAL = 0xFF
ADDI r1, r2, BANK_ADDR   ; Use variable as immediate
SW r1, r2, CONFIG_VAL    ; Variables work in all immediate fields
```
- **Syntax**: `NAME = value` (whitespace around `=` optional)
- **Value formats**: Hex (`0x123`) or decimal (`291`)
- **Bounds**: Values must fit in 12-bit immediate field (max 0xFFF)
- **Case insensitive**: `BANK_REG` and `bank_reg` refer to the same variable
- **Error on duplicate**: Duplicate variable definitions produce an error

## Signed vs Unsigned Immediates

The choice between signed and unsigned immediates is determined by **instruction type**, not by special syntax in the assembly code.

### Unsigned Instructions
These use `parse_immediate_unsigned()` and accept values from `0` to `max_value`:
- **I-type**: ADDI, SUBI, MULI, DIVI, ANDI, ORI, XORI, SLLI, SRLI
- **Stores**: SB, SH, SW
- **Loads**: LW, LH, LB
- **JALR**
- **LUI/AUIPC**: 16-bit immediates (0 to 0xFFFF)
- **SYS**: 8-bit immediates (0 to 0xFF)
- **CALL**: 20-bit immediates (0 to 0xFFFFF)

Example:
```asm
ADDI r1, r2, 0xFFF     ; Unsigned 12-bit
LUI r1, 0x1234         ; Unsigned 16-bit
SYS 0xFF                ; Unsigned 8-bit
```

### Signed Instructions
These use `parse_immediate_signed()` and accept values from `-2048` to `2047`:
- **Branches**: BEQ, BNE, BLT, BGT, BLE, BGE

Example:
```asm
BEQ r1, r2, -10         ; Signed branch offset
BLT r1, r2, loop        ; Label-based branch (resolved as signed offset)
```

### How It Works
The assembler automatically selects the appropriate parser based on the instruction:
1. Instruction type is determined from the opcode
2. For branches: `parse_immediate_signed()` is called via `parse_imm_or_label_signed()`
3. For others: `parse_immediate_unsigned()` is called via `parse_imm_or_label_unsigned()`

**Note**: When using labels in branch instructions, the assembler calculates the signed PC-relative offset automatically.

## Key Files
| File | Purpose |
|------|---------|
| `src/asm/asm_main.cpp` | Main assembler logic, two-pass, error handling |
| `src/asm/asm_utils.cpp` | Utility functions for parsing (labels, variables, comments) |
| `src/asm/assemble.cpp` | Instruction encoding, label resolution, variable resolution |
| `src/asm/instructions.cpp` | Instruction table (matches ISA) |
| `src/asm/rom_writer.cpp` | ROM file writing |
| `include/asm.h` | Assembler interface |
| `include/asm_utils.h` | Utility function declarations |
| `include/assemble.h` | Label and Variable structs and functions |

## Debug Output
When DEBUG is defined:
- `printf` outputs each instruction being assembled
- `write_to_file` outputs byte packing details

When DEBUG_LOG is defined:
- Errors logged to `<rom_path>.log` with line number and content

## Important Notes
- **No config.h**: Debug flags are passed via `-D` at build time
- **printf wrapped with #ifdef DEBUG**: Debug output only with debug builds
- **Labels**: Case insensitive, max 32 chars, global scope only
- **Variables**: Case insensitive, max 32 chars, 12-bit bounds (max 0xFFF)
- **Test suite must be updated**: When adding/removing instructions from `instructions.cpp`, update `roms/tests/` accordingly (add new pass tests, remove stale ones)
- **Variable tests**: When adding variable support, add tests in `tests/pass/` and `tests/fail/`

## Workflow
1. Edit source files in `src/asm/` or `include/`
2. Build with one of the 4 build scripts
3. Test with `./bash/run_asm.sh <input.asm> <output.rom>`
4. Debug builds show verbose output; release builds are silent

## Error Handling
Errors include line number and line content:
```
ERROR at line 5:
  BAD_INST r1, r2
  Unknown instruction
```