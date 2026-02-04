#ifndef VM_H
#define VM_H

#include "architecture.h"
#include <stdint.h>
#include <stdbool.h>

// Instruction decode helpers
typedef struct {
    uint8_t opcode;
    uint8_t funct3;
    uint8_t funct4;
    uint8_t opcode_funct3;
    uint8_t rd;
    uint8_t rs1;
    uint8_t rs2;
    uint16_t imm;
    Instruction* instruction;
} DecodedInstruction;

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

DecodedInstruction vm_decode(uint32_t instruction, Instruction instructionProperties);

#endif // VM_H
