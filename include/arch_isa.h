#ifndef ARCH_ISA_H
#define ARCH_ISA_H

#define REG_SP 13
#define REG_LR 14
#define REG_PC 15

#define IS_RESERVED_REG(r) ((r) >= REG_SP)

#define RESERVED_SYMBOLS \
    X(SP) \
    X(LR) \
    X(PC)

#define IS_RESERVED_SYMBOL(name) \
    (strcasecmp(name, "SP") == 0 || \
     strcasecmp(name, "LR") == 0 || \
     strcasecmp(name, "PC") == 0)

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