#ifndef ARCH_ISA_H
#define ARCH_ISA_H

#include <stdint.h>
#include <strings.h>
#include <string.h>

#define REG_SP 13
#define REG_LR 14
#define REG_PC 15

#define IS_RESERVED_REG(r) ((r) >= REG_SP)

typedef struct {
    const char *name;
    uint8_t reg_num;
} ReservedRegister;

#define RESERVED_REGISTERS \
    {"SP", REG_SP}, \
    {"LR", REG_LR}, \
    {"PC", REG_PC}

#define NUM_RESERVED_REGISTERS 3

#endif
