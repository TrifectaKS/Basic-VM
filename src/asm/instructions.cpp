#include "instructions.h"

Instruction instructions[] = {
    {"NOP", 0x0, 0x0, 0x0},

    {"ADD", 0x01, 0x0, 0x0},
    {"SUB", 0x01, 0x0, 0x1},
    {"MUL", 0x01, 0x0, 0x2},
    {"DIV", 0x01, 0x0, 0x3},

    {"ADDI", 0x01, 0x1, 0x0},
    {"SUBI", 0x01, 0x1, 0x1},
    {"MULI", 0x01, 0x1, 0x2},
    {"DIVI", 0x01, 0x1, 0x3},

    {"LUI", 0x02, 0x0, 0x0},
    {"AUIPC", 0x02, 0x0, 0x1},

    {"SB", 0x03, 0x0, 0x0},
    {"SH", 0x03, 0x0, 0x1},
    {"SW", 0x03, 0x0, 0x2},

    {"BEQ", 0x04, 0x0, 0x0},
    {"BNE", 0x04, 0x0, 0x1},
    {"BLT", 0x04, 0x0, 0x2},
    {"BGT", 0x04, 0x0, 0x3},
    {"BLE", 0x04, 0x0, 0x4},
    {"BGE", 0x04, 0x0, 0x5},

    {"JAL", 0x05, 0x0, 0x1},
    {"JALR", 0x05, 0x0, 0x2},

    {"LW", 0x06, 0x0, 0x0},
    {"LH", 0x06, 0x0, 0x1},
    {"LB", 0x06, 0x0, 0x2},

    {"AND", 0x07, 0x0, 0x0},
    {"OR", 0x07, 0x0, 0x1},
    {"XOR", 0x07, 0x0, 0x2},

    {"ANDI", 0x07, 0x1, 0x0},
    {"ORI", 0x07, 0x1, 0x1},
    {"XORI", 0x07, 0x1, 0x2},

    {"SLL", 0x08, 0x0, 0x0},
    {"SRL", 0x08, 0x0, 0x1},

    {"SLLI", 0x08, 0x1, 0x0},
    {"SRLI", 0x08, 0x1, 0x1},

    {"SYS", 0x09, 0x0, 0x0},
};

Instruction *get_instruction_by_alias(char *instructionStr) {
 size_t count = sizeof(instructions) / sizeof(instructions[0]);
  for (size_t i = 0; i < count; i++) {
    if (strcasecmp(instructionStr, instructions[i].name) == 0) {
      return &instructions[i];
    }
  }
  return NULL;
}

Instruction *get_instruction_by_asm(const char *asmLine) {
  char instructionStr[10];
  sscanf(asmLine, " %s", instructionStr);
  return get_instruction_by_alias(instructionStr);
}

Instruction *get_instruction_by_opcode(uint8_t opcode) {
  size_t count = sizeof(instructions) / sizeof(instructions[0]);
  for (size_t i = 0; i < count; i++) {
    if (instructions[i].opcode == opcode) {
      return &instructions[i];
    }
  }
  return NULL;
}

Instruction *get_instruction_by_all(uint8_t opcode, uint8_t funct3, uint8_t funct4) {
  size_t count = sizeof(instructions) / sizeof(instructions[0]);
  for (size_t i = 0; i < count; i++) {
    if (instructions[i].opcode == opcode &&
        instructions[i].funct3 == funct3 &&
        instructions[i].funct4 == funct4) {
      return &instructions[i];
    }
  }
  return NULL;
}