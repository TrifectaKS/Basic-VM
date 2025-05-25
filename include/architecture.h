#ifndef ARCHITECTURE_H
#define ARCHITECTURE_H

#include <stdint.h>

struct BasicVm
{
    uint8_t registers[16];
    uint8_t memory[4096];
    uint16_t program_counter;
    uint16_t opcode;
    uint16_t index_register;
    uint16_t stack[16];
    uint8_t stack_pointer;
    uint8_t delay_timer;
    uint8_t sound_timer;
};

#endif