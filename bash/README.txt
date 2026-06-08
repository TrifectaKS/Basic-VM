Basic-VM Assembler Build Scripts
=============================

build_asm_macos.sh
  Basic build, no optimization, no debug

build_asm_macos_opt.sh
  Size optimized (-Os -s -flto), no debug

build_asm_macos_debug.sh
  Debug enabled (-DDEBUG -DDEBUG_LOG), no optimization
  Enables printf output during assembly
  Creates error log files on failure

build_asm_macos_debug_opt.sh
  Size optimized + debug enabled

Usage:
  ./bash/build_asm_macos.sh
  ./bash/run_asm.sh <input.asm> <output.rom>