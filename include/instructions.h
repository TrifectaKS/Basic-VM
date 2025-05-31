#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#define BITS_0 0
#define BITS_4 4
#define BITS_12 12
#define BITS_16 16
#define BITS_20 20
#define BITS_24 24

#include <stdint.h>
#include <strings.h>
#include <stdio.h>

typedef struct
{
    char *name;
    uint8_t opcode;
    uint8_t funct3;
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
    {"ADD", 0x1, 0x2, 0x0, 24, BITS_4, BITS_4, BITS_4, BITS_4, BITS_0},
    {"SUB", 0x1, 0x4, 0x1, 24, BITS_4, BITS_4, BITS_4, BITS_4, BITS_0},
    {"MUL", 0x1, 0x3, 0x2, 24, BITS_4, BITS_4, BITS_4, BITS_4, BITS_0},
    {"DIV", 0x1, 0x5, 0x3, 24, BITS_4, BITS_4, BITS_4, BITS_4, BITS_0},
};

Instruction* get_instruction_by_alias(char *instructionStr)
{
    for (int i = 0; i < sizeof(instructions) / sizeof(instructions[0]); i++)
    {
        if (strcasecmp(instructionStr, instructions[i].name) == 0)
        {
            return &instructions[i];
        }
    }
}

Instruction *get_instruction_by_asm(const char* asmLine)
{
    char instructionStr[10];
    int count = sscanf(asmLine, " %s", instructionStr);  // Note the space before %s
    return get_instruction_by_alias(instructionStr);
}

Instruction *get_instruction_by_opcode_funct3(uint8_t opcode, uint8_t funct3)
{
    for (int i = 0; i < sizeof(instructions) / sizeof(instructions[0]); i++)
    {
        if (instructions[i].opcode == opcode && instructions[i].funct3 == funct3)
        {
            return &instructions[i];
        }
    }
}

#endif
