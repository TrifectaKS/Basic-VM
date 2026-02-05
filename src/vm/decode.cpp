#include "decode.h"
#include "config.h"
#include <string.h>

DecodedInstruction vm_decode(uint32_t instruction, Instruction instructionProperties) {
    DecodedInstruction dec = {0};

    dec.funct3 = instruction & 0x7;
    dec.opcode = (instruction >> 3) & 0x1F;

    dec.instruction = &instructionProperties;
    dec.opcode_funct3 = instructionProperties.opcode_funct3;

    // 24 BIT INSTRUCTIONS (ARITHMETIC, LOGIC, SHIFTS)
    // Note: SLLI/SRLI (opcode_funct3=0x51) are 32-bit, handled in switch below
    if (dec.opcode_funct3 == 0x08 || dec.opcode_funct3 == 0x40 || dec.opcode_funct3 == 0x50) {
        // 24-bit R-type instruction
        return decode_bitwise_rtype(instruction, instructionProperties);
    }

    // 32-bit instruction - decode based on opcode
    switch (dec.opcode) {
        case 0x01:
            return decode_arith_rtype(instruction, instructionProperties);
        case 0x02:
            return decode_arith_itype(instruction, instructionProperties);
        case 0x03:
            return decode_upper_imm(instruction, instructionProperties);
        case 0x04:
            return decode_store(instruction, instructionProperties);
        case 0x05:
            return decode_branch(instruction, instructionProperties);
        case 0x06:
            return decode_jump(instruction, instructionProperties);
        case 0x07:
            return decode_load(instruction, instructionProperties);
        case 0x09:
            return decode_logic_imm(instruction, instructionProperties);
        case 0x0A:
            return decode_shift_imm(instruction, instructionProperties);
        case 0x0F:
            return decode_halt(instruction, instructionProperties);
        default:
            // Default decode for unknown opcodes
            dec.funct4 = 0;
            dec.rd = (instruction >> 8) & 0xF;
            dec.rs1 = (instruction >> 12) & 0xF;
            dec.rs2 = (instruction >> 16) & 0xF;
            dec.imm = (instruction >> 16) & 0xFFFF;
            break;
    }

    return dec;
}

// Arithmetic R-type (ADD, SUB, MUL, DIV)
DecodedInstruction decode_arith_rtype(uint32_t instruction, Instruction ins) {
    DecodedInstruction dec = {0};
    dec.funct3 = instruction & 0x7;
    dec.opcode = (instruction >> 3) & 0x1F;
    dec.instruction = &ins;
    dec.opcode_funct3 = ins.opcode_funct3;

    uint8_t byte1 = (instruction >> 8) & 0xFF;
    uint8_t byte2 = (instruction >> 16) & 0xFF;

    dec.funct4 = byte1 & 0xF;
    dec.rd = byte1 >> 4;
    dec.rs1 = byte2 & 0xF;
    dec.rs2 = byte2 >> 4;
    dec.imm = 0;

    return dec;
}

// Arithmetic immediates (ADDI, SUBI, MULI, DIVI)
DecodedInstruction decode_arith_itype(uint32_t instruction, Instruction ins) {
    DecodedInstruction dec = {0};
    dec.funct3 = instruction & 0x7;
    dec.opcode = (instruction >> 3) & 0x1F;
    dec.instruction = &ins;
    dec.opcode_funct3 = ins.opcode_funct3;

    dec.funct4 = 0;
    dec.rd = (instruction >> 8) & 0xF;
    dec.rs1 = (instruction >> 12) & 0xF;
    dec.rs2 = 0;
    dec.imm = (instruction >> 16) & 0xFFFF;

    return dec;
}

// Upper immediates (LUI, AUIPC)
DecodedInstruction decode_upper_imm(uint32_t instruction, Instruction ins) {
    DecodedInstruction dec = {0};
    dec.funct3 = instruction & 0x7;
    dec.opcode = (instruction >> 3) & 0x1F;
    dec.instruction = &ins;
    dec.opcode_funct3 = ins.opcode_funct3;

    dec.funct4 = (instruction >> 8) & 0xF;
    dec.rd = (instruction >> 12) & 0xF;
    dec.rs1 = 0;
    dec.rs2 = 0;
    dec.imm = (instruction >> 16) & 0xFFFF;

    return dec;
}

// Store (SB, SH, SW)
DecodedInstruction decode_store(uint32_t instruction, Instruction ins) {
    DecodedInstruction dec = {0};
    dec.funct3 = instruction & 0x7;
    dec.opcode = (instruction >> 3) & 0x1F;
    dec.instruction = &ins;
    dec.opcode_funct3 = ins.opcode_funct3;

    dec.funct4 = 0;
    dec.rs1 = (instruction >> 8) & 0xF;
    dec.rs2 = (instruction >> 12) & 0xF;
    dec.rd = 0;
    dec.imm = (instruction >> 16) & 0xFFFF;

    return dec;
}

// Branch (BEQ, BNE, BLT, BGT, BLE, BGE)
DecodedInstruction decode_branch(uint32_t instruction, Instruction ins) {
    DecodedInstruction dec = {0};
    dec.funct3 = instruction & 0x7;
    dec.opcode = (instruction >> 3) & 0x1F;
    dec.instruction = &ins;
    dec.opcode_funct3 = ins.opcode_funct3;

    dec.funct4 = 0;
    dec.rs1 = (instruction >> 8) & 0xF;
    dec.rs2 = (instruction >> 12) & 0xF;
    dec.rd = 0;
    // Note: Immediate bytes are NOT swapped for little-endian (assembler bug)
    dec.imm = ((instruction >> 16) & 0xFF) | (((instruction >> 24) & 0xFF) << 8);

    return dec;
}

// Jumps (JAL, JALR)
DecodedInstruction decode_jump(uint32_t instruction, Instruction ins) {
    DecodedInstruction dec = {0};
    dec.funct3 = instruction & 0x7;
    dec.opcode = (instruction >> 3) & 0x1F;
    dec.instruction = &ins;
    dec.opcode_funct3 = ins.opcode_funct3;

    dec.funct4 = 0;
    dec.rd = (instruction >> 8) & 0xF;

    if (dec.funct3 == 0x02) { // JALR
        dec.rs1 = (instruction >> 12) & 0xF;
        dec.imm = (instruction >> 16) & 0xFFFF; // 16-bit
    } else { // JAL
        dec.rs1 = 0;
        // imm is 20 bits: bits 12-23 (12 bits) + bits 24-31 (8 bits)
        dec.imm = ((instruction >> 12) & 0xFFF) | (((instruction >> 24) & 0xFF) << 12);
    }
    dec.rs2 = 0;

    return dec;
}

// Load (LW, LH, LB)
DecodedInstruction decode_load(uint32_t instruction, Instruction ins) {
    DecodedInstruction dec = {0};
    dec.funct3 = instruction & 0x7;
    dec.opcode = (instruction >> 3) & 0x1F;
    dec.instruction = &ins;
    dec.opcode_funct3 = ins.opcode_funct3;

    dec.funct4 = 0;
    dec.rd = (instruction >> 8) & 0xF;
    dec.rs1 = (instruction >> 12) & 0xF;
    dec.rs2 = 0;
    dec.imm = (instruction >> 16) & 0xFFFF;

    return dec;
}

// Logic immediates (ANDI, ORI, XORI)
DecodedInstruction decode_logic_imm(uint32_t instruction, Instruction ins) {
    DecodedInstruction dec = {0};
    dec.funct3 = instruction & 0x7;
    dec.opcode = (instruction >> 3) & 0x1F;
    dec.instruction = &ins;
    dec.opcode_funct3 = ins.opcode_funct3;

    dec.funct4 = 0;
    dec.rd = (instruction >> 8) & 0xF;
    dec.rs1 = (instruction >> 12) & 0xF;
    dec.rs2 = 0;
    dec.imm = (instruction >> 16) & 0xFFFF;

    return dec;
}

// Shift immediates (SLLI, SRLI)
DecodedInstruction decode_shift_imm(uint32_t instruction, Instruction ins) {
    DecodedInstruction dec = {0};
    dec.funct3 = instruction & 0x7;
    dec.opcode = (instruction >> 3) & 0x1F;
    dec.instruction = &ins;
    dec.opcode_funct3 = ins.opcode_funct3;

    dec.funct4 = (instruction >> 8) & 0xF;
    dec.rd = (instruction >> 12) & 0xF;
    dec.rs1 = (instruction >> 24) & 0xF;
    dec.rs2 = 0;
    dec.imm = (instruction >> 16) & 0xFF;

    return dec;
}

// HALT
DecodedInstruction decode_halt(uint32_t instruction, Instruction ins) {
    DecodedInstruction dec = {0};
    dec.funct3 = instruction & 0x7;
    dec.opcode = (instruction >> 3) & 0x1F;
    dec.instruction = &ins;
    dec.opcode_funct3 = ins.opcode_funct3;
    dec.isHalt = true;
    dec.funct4 = ins.funct4;

    dec.rd = 0;
    dec.rs1 = 0;
    dec.rs2 = 0;
    dec.imm = 0;

    return dec;
}

// Bitwise R-type (AND, OR, XOR) - 24-bit format
DecodedInstruction decode_bitwise_rtype(uint32_t instruction, Instruction ins) {
    DecodedInstruction dec = {0};
    dec.funct3 = instruction & 0x7;
    dec.opcode = (instruction >> 3) & 0x1F;
    dec.instruction = &ins;
    dec.opcode_funct3 = ins.opcode_funct3;

    uint8_t byte1 = (instruction >> 8) & 0xFF;
    uint8_t byte2 = (instruction >> 16) & 0xFF;

    dec.funct4 = byte1 & 0xF;
    dec.rd = byte1 >> 4;
    dec.rs1 = byte2 & 0xF;
    dec.rs2 = byte2 >> 4;
    dec.imm = 0;

    return dec;
}
