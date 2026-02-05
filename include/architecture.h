#ifndef ARCHITECTURE_H
#define ARCHITECTURE_H

#include <stdint.h>

// Memory layout (64KB)
// 0x0000 - 0x0FFF: Font space (4KB)
// 0x1000 - 0xFEFF: Program ROM (60KB)
// 0xFF00 - 0xFFFF: Stack (256 bytes)
#define RAM_SIZE       65536
#define FONT_ADDR      0x0000
#define FONT_SIZE      4096
#define PROGRAM_ROM    0x1000
#define PROGRAM_SIZE   61440
#define STACK_ADDR     0xFF00
#define STACK_SIZE     256

struct BasicVm
{
    uint8_t registers[16];
    uint8_t memory[RAM_SIZE];
    uint16_t program_counter;
    uint16_t opcode;
    uint16_t index_register;
    uint16_t stack[16];
    uint8_t stack_pointer;
    uint8_t delay_timer;
    uint8_t sound_timer;
};

#endif