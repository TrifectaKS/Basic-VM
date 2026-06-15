#ifndef ASSEMBLE_H
#define ASSEMBLE_H

#include "debug_utils.h"
#include "instructions.h"
#include "arch_isa.h"
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <stdint.h>
#include <strings.h>
#include <string.h>
#include "uthash.h"

#define MAX_LABEL_LENGTH 32
#define MAX_VAR_NAME_LENGTH 32

typedef struct {
    char name[MAX_LABEL_LENGTH];
    uint32_t address;
    UT_hash_handle hh;
} Label;

typedef struct {
    char name[MAX_VAR_NAME_LENGTH];
    uint32_t value;
    UT_hash_handle hh;
} Variable;

void clear_labels(void);
int add_label(const char *name, uint32_t address);
Label *find_label(const char *name);
int is_label(const char *str);
uint32_t resolve_label(const char *name, uint32_t current_pc, bool is_branch);

void clear_variables(void);
int add_variable(const char *name, uint32_t value);
Variable *find_variable(const char *name);

int8_t register_to_byte(const char *reg);
uint32_t imm_to_word_unsigned(const char *immStr);
uint16_t imm_to_word_signed(const char *immStr);
int parse_immediate_signed(const char *immStr, int16_t *out_value);
int parse_immediate_unsigned(const char *immStr, uint16_t max_value, uint16_t *out_value);

int is_reserved_register(const char *regStr);
int validate_not_reserved(int8_t reg, const char *instr_name);

static inline uint32_t encode_base(const Instruction *instruction) {
    return (instruction->funct3 & 0x7) |
           ((instruction->opcode & 0x1F) << 3) |
           ((instruction->funct4 & 0xF) << 8);
}

static inline uint32_t encode_rd(uint8_t rd) {
    return (rd & 0xF) << 12;
}

static inline uint32_t encode_rs1(uint8_t rs1) {
    return (rs1 & 0xF) << 16;
}

static inline uint32_t encode_rs2(uint8_t rs2) {
    return (rs2 & 0xF) << 20;
}

static inline uint32_t encode_imm(int start_bit, int num_bits, uint32_t imm) {
    uint32_t mask = (1 << num_bits) - 1;
    return (imm & mask) << start_bit;
}

typedef struct {
  uint32_t value;
  bool hasValue;
} AssembledOperation;

const AssembledOperation InvalidOperation = { .value = 0, .hasValue = false };

AssembledOperation assemble_rtype(const Instruction *instruction, const char *asmLine);
AssembledOperation assemble_itype(const Instruction *instruction, const char *asmLine);
AssembledOperation assemble_store(const Instruction *instruction, const char *asmLine);
AssembledOperation assemble_branch(const Instruction *instruction, const char *asmLine, uint32_t current_pc);
AssembledOperation assemble_jal(const Instruction *instruction, const char *asmLine);
AssembledOperation assemble_jalr(const Instruction *instruction, const char *asmLine);
AssembledOperation assemble_lui(const Instruction *instruction, const char *asmLine);
AssembledOperation assemble_sys(const Instruction *instruction, const char *asmLine);
AssembledOperation assemble_nop(const Instruction *instruction, const char *asmLine);
AssembledOperation assemble_push(const Instruction *instruction, const char *asmLine);
AssembledOperation assemble_pop(const Instruction *instruction, const char *asmLine);
AssembledOperation assemble_call(const Instruction *instruction, const char *asmLine);
AssembledOperation assemble_ret(const Instruction *instruction, const char *asmLine);

#endif