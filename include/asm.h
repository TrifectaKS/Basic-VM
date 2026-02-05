#ifndef ASM_H
#define ASM_H

#include "assemble.h"
#include "instructions.h"
#include "rom_writer.h"

AssembledOperation handle_funct3_artihmetic(Instruction *instruction, char asmLineBuffer[256]);
AssembledOperation handle_funct3_immediates(Instruction *instruction, char asmLineBuffer[256]);
AssembledOperation handle_funct3_upper_immediates(Instruction *instruction, char asmLineBuffer[256]);
AssembledOperation handle_funct3_stores(Instruction *instruction, char asmLineBuffer[256]);
AssembledOperation handle_funct3_branches(Instruction *instruction, char asmLineBuffer[256]);
AssembledOperation handle_funct3_jumps(Instruction *instruction, char asmLineBuffer[256]);
AssembledOperation handle_funct3_loads(Instruction *instruction, char asmLineBuffer[256]);
AssembledOperation handle_funct3_bitwise(Instruction *instruction, char asmLineBuffer[256]);
AssembledOperation handle_funct3_bitwise_immediates(Instruction *instruction, char asmLineBuffer[256]);
AssembledOperation handle_funct3_shifts(Instruction *instruction, char asmLineBuffer[256]);
AssembledOperation handle_byte_instruction(Instruction *instruction, char asmLineBuffer[256]);
AssembledOperation handle_opcode(Instruction *instruction, char asmLineBuffer[256]);

#endif
