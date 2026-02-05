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

uint8_t register_to_byte(const char *reg);
uint32_t imm_to_word_unsigned(const char *immStr);
uint8_t imm_to_half_word_unsigned(const char *immStr);
uint16_t imm_to_word_signed(const char *immStr);

typedef struct {
  uint32_t value;
  bool hasValue;
} AssembledOperation;

const AssembledOperation InvalidOperation = { .value = 0, .hasValue = false };

AssembledOperation assemble_arithmetic_bitwise(const Instruction *instruction, const char *asmLine);
AssembledOperation assemble_immediates_loads(const Instruction *instruction, const char *asmLine);
AssembledOperation assemble_upper_immediates(const Instruction *instruction, const char *asmLine);
AssembledOperation assemble_jumps(const Instruction *instruction, const char *asmLine);
AssembledOperation assemble_jumps_register(const Instruction *instruction, const char *asmLine);
AssembledOperation assemble_stores_branches(const Instruction *instruction, const char *asmLine);
AssembledOperation assemble_shift_immediates(const Instruction *instruction, const char *asmLine);
AssembledOperation assemble_byte_instruction(const Instruction *instruction, const char *asmLine);

#endif
