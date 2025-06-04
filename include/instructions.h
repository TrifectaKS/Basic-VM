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
  uint8_t has_funct4;
  uint8_t rd;
  uint8_t rs1;
  uint8_t rs2;
  uint8_t imm;
} Instruction;

// Instruction data
Instruction instructions[] = {
    // Arithmetic
    {"ADD", 0x01, 0x00, 0x08, 0x00, 24, BITS_4, BITS_4, BITS_4, BITS_4, BITS_0},
    {"SUB", 0x01, 0x00, 0x08, 0x01, 24, BITS_4, BITS_4, BITS_4, BITS_4, BITS_0},
    {"MUL", 0x01, 0x00, 0x08, 0x02, 24, BITS_4, BITS_4, BITS_4, BITS_4, BITS_0},
    {"DIV", 0x01, 0x00, 0x08, 0x03, 24, BITS_4, BITS_4, BITS_4, BITS_4, BITS_0},

    // Immediates
    {"ADDI", 0x02, 0x01, 0x10, 0, 32, BITS_0, BITS_4, BITS_4, BITS_0, BITS_16},
    {"SUBI", 0x02, 0x02, 0x11, 0, 32, BITS_0, BITS_4, BITS_4, BITS_0, BITS_16},
    {"MULI", 0x02, 0x03, 0x12, 0, 32, BITS_0, BITS_4, BITS_4, BITS_0, BITS_16},
    {"DIVI", 0x02, 0x04, 0x13, 0, 32, BITS_0, BITS_4, BITS_4, BITS_0, BITS_16},
    {"LOAD", 0x02, 0x05, 0x14, 0, 32, BITS_0, BITS_4, BITS_4, BITS_0, BITS_16},

    // Upper Immediates
    {"LLUI",    0x03, 0x00, 0x18, 0x00, 32, BITS_4, BITS_4, BITS_0, BITS_0, BITS_16},
    {"HLUI",    0x03, 0x00, 0x18, 0x01, 32, BITS_4, BITS_4, BITS_0, BITS_0, BITS_16},
    {"LAUIPC",  0x03, 0x00, 0x18, 0x02, 32, BITS_4, BITS_4, BITS_0, BITS_0, BITS_16},
};

Instruction *get_instruction_by_alias(char *instructionStr) {
  for (int i = 0; i < sizeof(instructions) / sizeof(instructions[0]); i++) {
    if (strcasecmp(instructionStr, instructions[i].name) == 0) {
      return &instructions[i];
    }
  }
  return NULL;
}

Instruction *get_instruction_by_asm(const char *asmLine) {
  char instructionStr[10];
  int count =
      sscanf(asmLine, " %s", instructionStr); // Note the space before %s
  return get_instruction_by_alias(instructionStr);
}

Instruction *get_instruction_by_opcode_funct3(uint8_t opcode, uint8_t funct3) {
  for (int i = 0; i < sizeof(instructions) / sizeof(instructions[0]); i++) {
    if (instructions[i].opcode == opcode && instructions[i].funct3 == funct3) {
      return &instructions[i];
    }
  }
  return NULL;
}

#endif
