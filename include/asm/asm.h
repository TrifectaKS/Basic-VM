#ifndef ASM_H
#define ASM_H

#include "asm/assemble.h"
#include "asm/instructions.h"
#include "asm/rom_writer.h"

AssembledOperation handle_opcode(Instruction *instruction, char asmLineBuffer[256], uint32_t current_pc);

#endif