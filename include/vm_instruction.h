#ifndef VM_INSTRUCTION_H
#define VM_INSTRUCTION_H

#include "architecture.h"
#include "decode.h"
#include <stdint.h>
#include <stdbool.h>

// Execute a decoded instruction
void vm_execute_instruction(BasicVm *vm, DecodedInstruction dec, uint16_t next_pc);

#endif // VM_INSTRUCTION_H
