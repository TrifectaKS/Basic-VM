#ifndef VM_H
#define VM_H

#include "architecture.h"
#include "decode.h"
#include <stdint.h>
#include <stdbool.h>

// VM initialization
void vm_init(BasicVm *vm);

// ROM loading
bool vm_load_rom(BasicVm *vm, const char *filename);

// Execution
bool vm_run(BasicVm *vm);

// Single step execution
DecodedInstruction vm_step(BasicVm *vm);

// Debug output
void vm_print_state(BasicVm *vm);
void vm_print_instruction(BasicVm *vm, DecodedInstruction decodedInstruction);

#endif // VM_H
