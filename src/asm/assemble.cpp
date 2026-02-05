#include "assemble.h"

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

uint32_t imm_to_word_unsigned(const char *immStr) {
    if (immStr == NULL || *immStr == '\0') return 0x0000;

    char *endptr = NULL;
    unsigned long value = strtoul(immStr, &endptr, 0);

    return (uint32_t)value;
}

uint8_t imm_to_half_word_unsigned(const char *immStr) {
    if (immStr == NULL || *immStr == '\0') return 0x00;

    char *endptr = NULL;
    unsigned long value = strtoul(immStr, &endptr, 0);

    if (*endptr == '\0' && value <= 0xFFFF) {
        return (uint8_t)value;
    }

    return 0x00;
}

uint16_t imm_to_word_signed(const char *immStr) {
    size_t len = strlen(immStr);
    if (len == 0) return 0x0000;

    char *endptr = NULL;
    int64_t value = strtol(immStr, &endptr, 0);

    if (*endptr == '\0' && value >= -32768 && value <= 32767) {
        return (uint16_t)(int16_t)value;
    }

    return 0x0000;
}

AssembledOperation assemble_arithmetic_bitwise(const Instruction *instruction,
                                       const char *asmLine) {
  char instructionStr[10];
  char rdStr[5], rs1Str[5], rs2Str[5];

  int count = sscanf(asmLine, "%s %[^,], %[^,], %s", instructionStr, rdStr,
                     rs1Str, rs2Str);
  assert(count == 4);
  if (count != 4) {
    return (AssembledOperation){.hasValue = false};
  }

  uint32_t rd = register_to_byte(rdStr);
  uint32_t rs1 = register_to_byte(rs1Str);
  uint32_t rs2 = register_to_byte(rs2Str);
  uint32_t insOp = 0;

  insOp |= (instruction->funct3 & 0b00000111);
  insOp |= (instruction->opcode & 0b00011111) << 3;
  insOp |= (instruction->funct4 & 0b00001111) << 8;
  insOp |= (rd & 0b00001111) << 12;
  insOp |= (rs1 & 0b00001111) << 16;
  insOp |= (rs2 & 0b00001111) << 20;

#ifdef DEBUG
  printf("Parsed Assembly \n");
  print_binary32(insOp);
#endif

  return (AssembledOperation){.value = insOp, .hasValue = true};
}

AssembledOperation assemble_immediates_loads(const Instruction *instruction,
                                       const char *asmLine) {
  char instructionStr[20];
  char rdStr[5], rs1Str[5], immStr[10];

  int count = sscanf(asmLine, "%s %[^,], %[^,], %s", instructionStr, rdStr,
                     rs1Str, immStr);
  assert(count == 4);
  if (count != 4) {
    return (AssembledOperation){.hasValue = false};
  }

  uint32_t rd = register_to_byte(rdStr);
  uint32_t rs1 = register_to_byte(rs1Str);
  uint32_t imm = imm_to_word_unsigned(immStr);
  uint32_t insOp = 0;

  insOp |= (instruction->funct3 & 0b00000111);
  insOp |= (instruction->opcode & 0b00011111) << 3;
  insOp |= (rd & 0b00001111) << 8;
  insOp |= (rs1 & 0b00001111) << 12;
  insOp |= imm << 16;

#ifdef DEBUG
  printf("Parsed Assembly \n");
  print_binary32(insOp);
#endif

  return (AssembledOperation){.value = insOp, .hasValue = true};
}

AssembledOperation assemble_upper_immediates(const Instruction *instruction,
                                             const char *asmLine) {
  char rdStr[5], immStr[10];

  int count = sscanf(asmLine, "%*s %[^,], %s", rdStr, immStr);
  if (count != 2) {
    return (AssembledOperation){.hasValue = false};
  }

  uint32_t rd = register_to_byte(rdStr);
  uint32_t imm = imm_to_word_unsigned(immStr) & 0xFFFF;

  uint32_t insOp = 0;
  insOp |= (instruction->funct3 & 0b00000111);
  insOp |= (instruction->opcode & 0b00011111) << 3;
  insOp |= (instruction->funct4 & 0b00001111) << 8;
  insOp |= (rd & 0b00001111) << 12;
  insOp |= imm << 16;

  return (AssembledOperation){.value = insOp, .hasValue = true};
}

AssembledOperation assemble_jumps(const Instruction *instruction,
                                             const char *asmLine) {
  char rdStr[5], immStr[10];

  int count = sscanf(asmLine, "%*s %[^,], %s", rdStr, immStr);
  if (count != 2) {
    return (AssembledOperation){.hasValue = false};
  }

  uint32_t rd = register_to_byte(rdStr);
  uint32_t imm = imm_to_word_unsigned(immStr) & 0xFFFFF;

  uint32_t insOp = 0;
  insOp |= (instruction->funct3 & 0b00000111);
  insOp |= (instruction->opcode & 0b00011111) << 3;
  insOp |= (rd & 0b00001111) << 8;
  insOp |= (imm & 0xFFF) << 12;
  insOp |= ((imm >> 12) & 0xFF) << 24;

#ifdef DEBUG
  printf("Parsed Assembly \n");
  print_binary32(insOp);
#endif

  return (AssembledOperation){.value = insOp, .hasValue = true};
}

AssembledOperation assemble_jumps_register(const Instruction *instruction,
                                                   const char *asmLine) {
  char rdStr[5], rs1Str[5], immStr[10];

  int count = sscanf(asmLine, "%*s %[^,], %[^,], %s", rdStr, rs1Str, immStr);
  if (count != 3) {
    return (AssembledOperation){.hasValue = false};
  }

  uint32_t rd = register_to_byte(rdStr);
  uint32_t rs1 = register_to_byte(rs1Str);
  uint32_t imm = imm_to_word_unsigned(immStr) & 0xFFFF;

  uint32_t insOp = 0;
  insOp |= (instruction->funct3 & 0b00000111);
  insOp |= (instruction->opcode & 0b00011111) << 3;
  insOp |= (rd & 0b00001111) << 8;
  insOp |= (rs1 & 0b00001111) << 12;
  insOp |= imm << 16;

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

  int count = sscanf(asmLine, "%s %[^,], %[^,], %s", instructionStr, rs1Str,
                     rs2Str, immStr);
  assert(count == 4);
  if (count != 4) {
    return (AssembledOperation){.hasValue = false};
  }

  uint32_t rs1 = register_to_byte(rs1Str);
  uint32_t rs2 = register_to_byte(rs2Str);
  uint32_t imm = imm_to_word_unsigned(immStr);
  uint32_t insOp = 0;

  insOp |= (instruction->funct3 & 0b00000111);
  insOp |= (instruction->opcode & 0b00011111) << 3;
  insOp |= (rs1 & 0b00001111) << 8;
  insOp |= (rs2 & 0b00001111) << 12;
  insOp |= imm << 16;

#ifdef DEBUG
  printf("Parsed Assembly \n");
  print_binary32(insOp);
#endif

  return (AssembledOperation){.value = insOp, .hasValue = true};
}
