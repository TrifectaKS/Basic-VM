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
- **Two-pass assembly**: Labels collected in pass 1, resolved in pass 2
- **Label support**: Global labels for branches/jumps (32 chars max, case insensitive)
- **Error logging**: When DEBUG_LOG is defined, errors written to `<output>.log`
- **Output format**: 32-bit instructions, little-endian

## Key Files
| File | Purpose |
|------|---------|
| `src/asm/asm_main.cpp` | Main assembler logic, two-pass, error handling |
| `src/asm/assemble.cpp` | Instruction encoding, label resolution |
| `src/asm/instructions.cpp` | Instruction table (matches ISA) |
| `src/asm/rom_writer.cpp` | ROM file writing |
| `include/asm.h` | Assembler interface |
| `include/assemble.h` | Label structs and functions |

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
- **Test suite must be updated**: When adding/removing instructions from `instructions.cpp`, update `roms/tests/` accordingly (add new pass tests, remove stale ones)

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