#ifndef DECODE_H
#define DECODE_H

#include "instructions.h"
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
    uint32_t imm;
    bool isHalt;
    Instruction* instruction;
} DecodedInstruction;

// Main decode function
DecodedInstruction vm_decode(uint32_t instruction, Instruction instructionProperties);

// Individual opcode decode functions
DecodedInstruction decode_arith_rtype(uint32_t instruction, Instruction ins);
DecodedInstruction decode_arith_itype(uint32_t instruction, Instruction ins);
DecodedInstruction decode_upper_imm(uint32_t instruction, Instruction ins);
DecodedInstruction decode_store(uint32_t instruction, Instruction ins);
DecodedInstruction decode_branch(uint32_t instruction, Instruction ins);
DecodedInstruction decode_jump(uint32_t instruction, Instruction ins);
DecodedInstruction decode_load(uint32_t instruction, Instruction ins);
DecodedInstruction decode_logic_imm(uint32_t instruction, Instruction ins);
DecodedInstruction decode_shift_imm(uint32_t instruction, Instruction ins);
DecodedInstruction decode_halt(uint32_t instruction, Instruction ins);
DecodedInstruction decode_bitwise_rtype(uint32_t instruction, Instruction ins);

#endif // DECODE_H
