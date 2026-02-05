#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#define BITS_0 0
#define BITS_4 4
#define BITS_12 12
#define BITS_16 16
#define BITS_20 20
#define BITS_24 24

#include <stdint.h>
#include <stdio.h>
#include <strings.h>

typedef struct {
  const char *name;
  uint8_t opcode;
  uint8_t funct3;
  uint8_t opcode_funct3;
  uint8_t funct4;
  uint8_t length;
} Instruction;

extern Instruction instructions[];

Instruction *get_instruction_by_alias(char *instructionStr);
Instruction *get_instruction_by_asm(const char *asmLine);
Instruction *get_instruction_by_opcode_funct3(uint8_t opcode, uint8_t funct3);
Instruction *get_instruction_by_opcodefunct3(uint8_t opcodefunct3);
Instruction *get_instruction_by_all(uint8_t opcode, uint8_t funct3, uint8_t funct4);

#endif
