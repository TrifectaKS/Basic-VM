#ifndef ASM_H
#define ASM_H

#include "assemble.h"
#include "instructions.h"
#include "rom_writer.h"

AssembledOperation handle_opcode(Instruction *instruction, char asmLineBuffer[256], uint32_t current_pc);

#endif