#ifndef ASSEMBLE_H
#define ASSEMBLE_H

#include "debug_utils.h"
#include "instructions.h"
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <stdint.h>
#include <strings.h>
#include <string.h>

uint8_t register_to_byte(const char *reg) {
  if (reg[0] == 'r' || reg[0] == 'R') {
    int value = atoi(reg + 1);
    if (value >= 0 && value <= 15) {
      printf("byte: %d \n", value);
      return value;
    }
  }

  return 0x0;
}

uint16_t imm_to_word(const char *immStr) {
  size_t len = strlen(immStr);
  if (len >= 4) {
    // Extract the string inside the quotes
    char number_str[7] = {0}; // max "0xFFFF" + null
    strncpy(number_str, immStr, len);
    number_str[len] = '\0'; // ensure null-terminated

    // Validate hex prefix
    printf("%s\n", number_str);
    if (strncmp(number_str, "0x", 2) == 0 ||
        strncmp(number_str, "0X", 2) == 0) {
      char *endptr = NULL;
      uint16_t value = strtol(number_str, &endptr, 16);
      if (*endptr == '\0' && value >= 0 && value <= 0xFFFF) {
        return (uint16_t)value;
      }
    }
  }
  return 0x0000; // Invalid input or out of range
}

typedef struct {
  uint32_t value;
  bool hasValue;
} AssembledOperation;

AssembledOperation assemble_arithmetic(const Instruction *instruction,
                                       const char *asmLine) {
  char instructionStr[10];
  char rdStr[5], rs1Str[5], rs2Str[5];

  // TODO: This depends on instruction
  int count = sscanf(asmLine, "%s %[^,], %[^,], %s", instructionStr, rdStr,
                     rs1Str, rs2Str);
  assert(count == 4);
  if (count != 4) {
    return (AssembledOperation){.hasValue = false};
  }

  // opcode(4)/funct3(3)/funct4(4)/rd(4)/rs1(4)/rs2(4) in uint32_t
  uint32_t rd = register_to_byte(rdStr);   // 4 bits
  uint32_t rs1 = register_to_byte(rs1Str); // 4 bits
  uint32_t rs2 = register_to_byte(rs2Str); // 4 bits
  uint32_t insOp = 0;

  insOp |= (instruction->funct3 & 0b00000111); // bits 0–2 (funct3)
  print_binary32(insOp);
  insOp |= (instruction->opcode & 0b00011111) << 3; // bits 3–7 (opcode)
  print_binary32(insOp);
  insOp |= (instruction->funct4 & 0b00001111) << 8; // bits 8–11 (funct4)
  print_binary32(insOp);
  insOp |= (rd & 0b00001111) << 12; // bits 12–15 (rd)
  print_binary32(insOp);
  insOp |= (rs1 & 0b00001111) << 16; // bits 16–19 (rs1)
  print_binary32(insOp);
  insOp |= (rs2 & 0b00001111) << 20; // bits 20–23 (rs2)

#ifdef DEBUG
  printf("Parsed Assembly \n");
  print_binary32(insOp);
#endif

  return (AssembledOperation){.value = insOp, .hasValue = true};
}

AssembledOperation assemble_immediates(const Instruction *instruction,
                                       const char *asmLine) {
  char instructionStr[20];
  char rdStr[5], rs1Str[5], immStr[10];

  // TODO: This depends on instruction
  int count = sscanf(asmLine, "%s %[^,], %[^,], %s", instructionStr, rdStr,
                     rs1Str, immStr);
  assert(count == 4);
  if (count != 4) {
    return (AssembledOperation){.hasValue = false};
  }

  // opcode(4)/funct3(3)/funct4(4)/rd(4)/rs1(4)/rs2(4) in uint32_t
  uint32_t rd = register_to_byte(rdStr);   // 4 bits
  uint32_t rs1 = register_to_byte(rs1Str); // 4 bits
  uint32_t imm = imm_to_word(immStr);      // 16 bits
  uint32_t insOp = 0;

  insOp |= (instruction->funct3 & 0b00000111); // bits 0–2 (funct3)
  print_binary32(insOp);
  insOp |= (instruction->opcode & 0b00011111) << 3; // bits 3–7 (opcode)
  print_binary32(insOp);
  insOp |= (rd & 0b00001111) << 8; // bits 8–11 (rd)
  print_binary32(insOp);
  insOp |= (rs1 & 0b00001111) << 12; // bits 12–16 (rs1)
  print_binary32(insOp);
  insOp |= imm << 16; // bits 16–31 (imm)

#ifdef DEBUG
  printf("Parsed Assembly \n");
  print_binary32(insOp);
#endif

  return (AssembledOperation){.value = insOp, .hasValue = true};
}

AssembledOperation assemble_upper_immediates_jumps(const Instruction *instruction,
                                             const char *asmLine) {
  char instructionStr[20];
  char rdStr[5], immStr[10];

  // TODO: This depends on instruction
  int count = sscanf(asmLine, "%s %[^,], %s", instructionStr, rdStr, immStr);
  assert(count == 3);
  if (count != 3) {
    return (AssembledOperation){.hasValue = false};
  }

  // opcode(4)/funct3(3)/funct4(4)/rd(4)/rs1(4)/rs2(4) in uint32_t
  uint32_t rd = register_to_byte(rdStr);   // 4 bits
  uint32_t imm = imm_to_word(immStr);      // 16 bits
  uint32_t insOp = 0;

  insOp |= (instruction->funct3 & 0b00000111); // bits 0–2 (funct3)
  print_binary32(insOp);
  insOp |= (instruction->opcode & 0b00011111) << 3; // bits 3–7 (opcode)
  print_binary32(insOp);
  insOp |= (rd & 0b00001111) << 8; // bits 8–11 (rd)
  print_binary32(insOp);
  insOp |= (instruction->funct4 & 0b00001111) << 12; // bits 12–16 (rs1)
  print_binary32(insOp);
  insOp |= imm << 16; // bits 16–31 (imm)

#ifdef DEBUG
  printf("Parsed Assembly \n");
  print_binary32(insOp);
#endif

  return (AssembledOperation){.value = insOp, .hasValue = true};
}

AssembledOperation assemble_stores_branches(const Instruction *instruction,
                                       const char *asmLine) {
  char instructionStr[20];
  char rs1Str[5], rs2Str[5], immStr[10];

  // TODO: This depends on instruction
  int count = sscanf(asmLine, "%s %[^,], %[^,], %s", instructionStr, rs1Str,
                     rs2Str, immStr);
  assert(count == 4);
  if (count != 4) {
    return (AssembledOperation){.hasValue = false};
  }

  // opcode(4)/funct3(3)/funct4(4)/rd(4)/rs1(4)/rs2(4) in uint32_t
  uint32_t rs1 = register_to_byte(rs1Str);   // 4 bits
  uint32_t rs2 = register_to_byte(rs2Str); // 4 bits
  uint32_t imm = imm_to_word(immStr);      // 16 bits
  uint32_t insOp = 0;

  insOp |= (instruction->funct3 & 0b00000111); // bits 0–2 (funct3)
  print_binary32(insOp);
  insOp |= (instruction->opcode & 0b00011111) << 3; // bits 3–7 (opcode)
  print_binary32(insOp);
  insOp |= (rs1 & 0b00001111) << 8; // bits 8–11 (rd)
  print_binary32(insOp);
  insOp |= (rs2 & 0b00001111) << 12; // bits 12–16 (rs1)
  print_binary32(insOp);
  insOp |= imm << 16; // bits 16–31 (imm)

#ifdef DEBUG
  printf("Parsed Assembly \n");
  print_binary32(insOp);
#endif

  return (AssembledOperation){.value = insOp, .hasValue = true};
}

#endif
