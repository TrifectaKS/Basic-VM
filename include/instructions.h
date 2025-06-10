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

// Instruction data
Instruction instructions[] = {
    {"NULL", 0x0, 0x0, 0x0, 0x0, 0x0},

    // Arithmetic
    {"ADD", 0x01, 0x00, 0x08, 0x00, 24},
    {"SUB", 0x01, 0x00, 0x08, 0x01, 24},
    {"MUL", 0x01, 0x00, 0x08, 0x02, 24},
    {"DIV", 0x01, 0x00, 0x08, 0x03, 24},

    // Immediates
    {"ADDI", 0x02, 0x00, 0x10, 0, 32},
    {"SUBI", 0x02, 0x01, 0x11, 0, 32},
    {"MULI", 0x02, 0x02, 0x12, 0, 32},
    {"DIVI", 0x02, 0x03, 0x13, 0, 32},

    // Upper Immediates
    {"LUI", 0x03, 0x00, 0x18, 0x00, 32},
    {"AUIPC", 0x03, 0x00, 0x18, 0x01, 32},

    // Stores
    {"SB", 0x04, 0x00, 0x20, 0, 32},
    {"SH", 0x04, 0x01, 0x21, 0, 32},
    {"SW", 0x04, 0x02, 0x22, 0, 32},

    // Branches
    {"BEQ", 0x05, 0x00, 0x28, 0, 32},
    {"BNE", 0x05, 0x01, 0x29, 0, 32},
    {"BLT", 0x05, 0x02, 0x2A, 0, 32},
    {"BGT", 0x05, 0x03, 0x2B, 0, 32},
    {"BLE", 0x05, 0x04, 0x2C, 0, 32},
    {"BGE", 0x05, 0x05, 0x2D, 0, 32},

    // Jump + Link
    {"JAL", 0x06, 0x00, 0x31, 0x00, 32},
    {"JALR", 0x06, 0x00, 0x31, 0x01, 32},

    // Loads
    {"LW", 0x07, 0x00, 0x38, 0, 32},
    {"LH", 0x07, 0x01, 0x39, 0, 32},
    {"LB", 0x07, 0x02, 0x3A, 0, 32},
};

Instruction *get_instruction_by_alias(char *instructionStr) {
  for (int i = 0; i < sizeof(instructions) / sizeof(instructions[0]); i++) {
    if (strcasecmp(instructionStr, instructions[i].name) == 0) {
      return &instructions[i];
    }
  }
  return &instructions[0];
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
